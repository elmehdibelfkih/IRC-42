/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:33 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/03 18:40:58 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

Server::Server()
{
    this->_passWord = "";
    this->_port = -1;
}

Server &Server::operator=(const Server &obj)
{
    if (this != &obj)
    {
        this->_port = obj._port;
        this->_passWord = obj._passWord;
        this->_fds = obj._fds;
        this->_clients = obj._clients;
        this->_channels = obj._channels;
    }
    return *this;
}

Server::Server(const Server &obj)
{
    *this = obj;
}

Server::~Server()
{
    this->_passWord.clear();
    this->_fds.clear();
    this->_clients.clear();
    this->_channels.clear();
}


Server::Server(std::string port, std::string password) : _passWord(password)
{
    int p;
    char *end;

    p = strtod(port.c_str(), &end);
    if (!port.find('.') || strcmp("", end) || !(1024 < p && p < 49151))
    {
        std::cerr << "ERROR: bad input" << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_port = htons(p);
}

void Server::startServer()
{
    int fdSocket;
    struct sockaddr_in serverAddr;

    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket < 0)
    {
        std::cerr << "Error: creating socket: " << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    int opt = 1;
	if (setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "error set server socket to reuseaddress" << std::endl;
        close(fdSocket);
        exit(EXIT_FAILURE);        
    }
    if (fcntl(fdSocket, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "error set server socket to non-blocking" << std::endl;
        close(fdSocket);
        exit(EXIT_FAILURE);
    }
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = this->_port;
    serverAddr.sin_family = AF_INET;
    if (bind(fdSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error binding socket, maybe the port was already used" << std::endl;
        close(fdSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(fdSocket, SOMAXCONN) < 0)
    {
        std::cerr << "listen() failed" << std::endl;
        close(fdSocket);
        exit(EXIT_FAILURE);
    }
    pollfd tmp;
    tmp.fd = fdSocket;
    tmp.events = POLLIN;
    this->_fds.push_back(tmp);
    this->handleClientConnection();
}



void Server::handleClientConnection()
{
    while (true)
    {
        int NReady = poll(&(this->_fds[0]), this->_fds.size(), -1);
        if (NReady < 0)
        {
            std::cerr << "poll() failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        // Check for new connections
        if (this->_fds[0].revents & POLLIN)
        {
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            int clientFdSocket = accept(this->_fds[0].fd, (struct sockaddr *)&clientAddr, &addrLen);
            if (clientFdSocket < 0) {
                std::cerr << "accept() failed" << std::endl;
                continue;
            }
            if (fcntl(clientFdSocket, F_SETFL, O_NONBLOCK) == -1) {
                std::cerr << "failed set client socket to non-blocking" << std::endl;
                close(clientFdSocket);
                continue;
            }
            std::string clientIP = inet_ntoa(clientAddr.sin_addr);
            std::cout << "New client connected: " << clientIP << std::endl;

            pollfd tmp;
            tmp.fd = clientFdSocket;
            tmp.events = POLLIN | POLLOUT;

            this->_fds.push_back(tmp);
            Client Ctmp(clientFdSocket, false);
            Ctmp.setIP(clientIP);
            this->_clients[clientFdSocket] = Ctmp;
        }

        // Check for data on client sockets
        for (size_t i = 1; i < this->_fds.size(); i++)
        {
            if (this->_fds[i].revents & POLLIN)
            {
                // Message 
                int bytesReceived;
                char buffer[1024];
                memset(buffer, 0, 1024);
                bytesReceived = recv(this->_fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesReceived == 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    // this->_clients[this->_fds[i].fd].disconnect();
                    if (this->_clients[this->_fds[i].fd].getAuthenticate())
                    {
                        for ( std::map<std::string, Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
                        (  *it).second.removeClient(_clients[this->_fds[i].fd], -1); 
                    }
                    close(this->_fds[i].fd);
                    this->_clients.erase(this->_fds[i].fd);
                    this->_fds.erase(this->_fds.begin() + i);
                    continue;
                }
                if (bytesReceived < 0) {
                    std::cerr << "recv() failed" << std::endl;
                    continue;
                }
                this->_clients[this->_fds[i].fd].consume_message(buffer);

            }
            if (this->_clients[this->_fds[i].fd].getMessage().IsReady()) {
                this->handleClientMessage(this->_fds[i].fd);
            }
            if (this->_fds[i].revents & POLLOUT)
                this->_clients[this->_fds[i].fd].writeMessageToSocket();
        }

    }
}

void Server:: handleClientMessage(int i)
{
        if (this->authenticateUser(i))
        {
            switch (this->_clients[i].getMessage().getCommand())
            {
            case (JOIN):
                joinCommand(i);
                break;
            case (LIST):
                listCommand(i);
                break;
            case (PART):
                partCommand(i);
                break;
            case (TOPIC):
                topicCommand(i);
                break;
            case (PRIVMSG):
                privmsgCommand(i);
                break;
            case (INVITE):
                inviteCommand(i);
                break;
            case (KICK):
                kickCommand(i);
                break;
            case (MODE):
                modeCommand(i);
                break;
            case (NICK):
                nickCommand(i);
                break;
            case PASS:case USER:case PONG:case QUIT:
                break;
            default:
                this->_clients[i].sendMsg(ERR_UNKNOWNCOMMAND(_clients[i].getNickName()));
            }
        }
}

bool Server::authenticateUser(int i)
{

    if (this->_clients[i].getAuthenticate())
        return true;
    else if (this->_clients[i].getMessage().getCommand() == PASS)
        this->passCommand(i);
    else if (this->_clients[i].getMessage().getCommand() == NICK)
        this->nickCommand(i);
    else if (!this->_clients[i].getNickName().empty() && this->_clients[i].getMessage().getCommand() == USER)
        this->userCommand(i);
    else
        this->_clients[i].sendMsg(ERR_NOTREGISTERED("*"));

    return false;
}

Client *Server::getClientByNickName(std::string nick)
{
    for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
    {
        if (it->second.getNickName() == nick)
            return &it->second;
    }
    return NULL;
}

bool Server::checkUserName(std::string username)
{
    if(username.empty())
        return false;

    for (size_t i = 0; i < username.size(); ++i)
    {
        char c = username[i];
        if (!(isalpha(c) || isdigit(c) ))
            return false;
    }
    return true;
}

bool Server::checkNickName(std::string nickname)
{
        char c = nickname.at(0);
        if (!(isalpha(c)))
            return false;
    return true;
}

void Server::createChannel(std::string &channelname, std::string key)
{
    this->_channels.insert(std::pair<std::string, Channel>(channelname, Channel(channelname, key)));
    if (!key.empty())
        this->_channels[channelname]._mode.requiredKey = true;

}

bool Server::findChannelName(std::string &channelName)
{

    if (this->_channels.empty() || channelName.empty())
        return (false);
    std::map<std::string, Channel>::iterator it = this->_channels.find(channelName);  
    return (it != this->_channels.end() );

}

bool Server::is_memberInChannel(std::string &channelName, Client cl)
{
    std::map<std::string, Client*>::iterator it = this->_channels[channelName]._clients.find(cl.getNickName());
    if (it != _channels[channelName]._clients.end() && it->first == cl.getNickName())
        return (true);
    return (false);
}

bool Server::isValidChannelKey(std::string &key)
{
    if (key.empty())
        return (false);
    if (key.size() < 4 || key.size() >= 32)
        return (false);
    for (size_t i = 0; key.size() > i; i++)
    {
        if (key[i] == ' ' || (key.at(i) >= 9 && key.at(i) <= 13))
            return (false);
    }
    return (true);
}


bool Server::isValidChannelName(std::string &channelName)
{
    if (channelName.empty())
        return (false);
    if (channelName.at(0) != '#')
        return (false);
    if (!(channelName.size() >= 2 && channelName.size() <= CHANNELNAMELEN))
        return (false);
    size_t pos = channelName.find_first_of(",:?*!@ ");
    if (pos != std::string::npos) {
        return (false);
    }
    return (true);
}


