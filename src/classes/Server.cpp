/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:33 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/24 17:01:33 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"

Server::Server()
{
    this->_passWord = "";
    this->_port = -1;
}

Server& Server::operator=(const Server& obj)
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

Server::Server(const Server& obj)
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
 
///////////////////////////////////////////////////////////////////////////////////

Server::Server(std::string port, std::string password) : _passWord(password)
{
    int p;
    char *end;

    p = strtod(port.c_str(), &end);
    if (!port.find('.')  || strcmp("", end) || !(1024 < p && p < 49151))
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
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = this->_port;
    serverAddr.sin_family = AF_INET;
    if (bind(fdSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
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
        if (NReady < 0){
            std::cerr << "poll() failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        // Check for new connections
        if (this->_fds[0].revents & POLLIN){
            int clientFdSocket = accept(this->_fds[0].fd, NULL, NULL);
            if (clientFdSocket < 0)
            std::cerr << "accept() failed" << std::endl;
            else {
                std::cout << "New client connected" << std::endl;
                pollfd tmp;
                tmp.fd = clientFdSocket;
                tmp.events = POLLIN;
                this->_fds.push_back(tmp);
                Client Ctmp(clientFdSocket, false);
                this->_clients[clientFdSocket] = Ctmp;
            }
        }
        
        // Check for data on client sockets
        for (size_t i = 1; i < this->_fds.size(); i++){
            if (this->_fds[i].revents & POLLIN)
            {
                Message msg;
                int bytesReceived;
                char buffer[4096];
                memset(buffer, 0, 4096);
                bytesReceived = recv(this->_fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesReceived == 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    close(this->_fds[i].fd);
                    this->_clients[this->_fds[i].fd].disconnect();
                    this->_clients.erase(this->_fds[i].fd);
                    this->_fds.erase(this->_fds.begin() + i);
                    break;
                }
                if (bytesReceived < 0)
                    std::cerr << "recv() failed" << std::endl;
                else
                {
                    msg = msg + buffer;
                    this->_clients[this->_fds[i].fd].setMessage(msg);
                    this->handleClientMessage(this->_fds[i].fd);
                }
            }
        }
    }
}

void Server::handleClientMessage(int i)
{
    if (this->_clients[i].getMessage().IsReady())
    {
        if (this->authenticateUser(i))
        {
            std::cout << "mrhbabik\n";
            // (.............) << youssef
        }
        this->_clients[i].getMessage().clearBuffer();
    }
}

bool Server::authenticateUser(int i)
{
    if (this->_clients[i].getAuthenticate())
        return true;
    else if (!this->_clients[i].getPass())
    {
        if (this->_clients[i].getMessage().getCommand() == PASS)
        {
            if (this->_clients[i].getMessage().getToken().size() == 0)
                this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS((std::string)"x",(std::string)"pass"));
            else if (this->_clients[i].getPass() == true)
                this->_clients[i].sendMsg(ERR_ALREADYREGISTERED((std::string)"x"));
            else if (this->_clients[i].getMessage().getToken() == this->_passWord)
                this->_clients[i].setPass(true);
            else
                this->_clients[i].sendMsg(ERR_PASSWDMISMATCH((std::string)"x"));
        }
        else
             this->_clients[i].sendMsg(ERR_NOTREGISTERED((std::string)"x"));
    }
    else if (this->_clients[i].getNickName().size() == 0)
    {
        if (this->_clients[i].getMessage().getCommand() == NICK)
        {
            if (this->_clients[i].getMessage().getToken().size() == 0)
                this->_clients[i].sendMsg(ERR_NONICKNAMEGIVEN((std::string)"x"));
            else if (this->getClientByNickName(this->_clients[i].getMessage().getToken()) != NULL)
                this->_clients[i].sendMsg(ERR_NICKNAMEINUSE((std::string)"x",this->_clients[i].getMessage().getToken()));
            else if (!this->checkNickName(i))
                this->_clients[i].sendMsg(ERR_ERRONEUSNICKNAME((std::string)"x",this->_clients[i].getMessage().getToken()));
        }
        else
             this->_clients[i].sendMsg(ERR_NOTREGISTERED((std::string)"x"));
    }
    return false;
}

Client* Server::getClientByNickName(std::string nick)
{
    for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
    {
        if (it->second.getNickName() == nick)
            return &it->second;
    }
    return NULL;
}

bool Server::checkNickName(int i)
{
    if (this->_clients[i].getMessage().getToken().find(' ') != std::string::npos || this->_clients[i].getMessage().getToken().find(',') != std::string::npos 
            || this->_clients[i].getMessage().getToken().find('*') != std::string::npos ||  this->_clients[i].getMessage().getToken().find('!') != std::string::npos 
            || this->_clients[i].getMessage().getToken().find('?') != std::string::npos || this->_clients[i].getMessage().getToken().find('@') != std::string::npos 
            || this->_clients[i].getMessage().getToken().find('.') != std::string::npos)
        return false;
    if (*this->_clients[i].getMessage().getToken().begin() == ':' || *this->_clients[i].getMessage().getToken().begin() == '$')
        return false;
        this->_clients[i].setNickName(this->_clients[i].getMessage().getToken());
    return true;
}
