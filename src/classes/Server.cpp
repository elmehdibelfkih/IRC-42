/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:33 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/26 13:43:33 by ybouchra         ###   ########.fr       */
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



// void disconnect()
// {


// }
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
                // else
                // {

                //    this->_clients[this->_fds[i].fd].setMessage()
                    
                    // msg = msg + buffer;
                    // this->handleClientMessage(this->_fds[i].fd);
                // }
            }
            if (this->_clients[this->_fds[i].fd].getMessage().IsReady()) {
                this->handleClientMessage(this->_fds[i].fd);
            }
            if (this->_fds[i].revents & POLLOUT)
                this->_clients[this->_fds[i].fd].writeMessageToSocket();
        }
        // for (size_t i = 1; i < this->_fds.size(); i++) {
        //     if (this->_fds[i].revents & POLLOUT) {
        //     }
        // }
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
        return (0);
    if (channelName.at(0) != '#')
        return (0);
    if (!(channelName.size() >= 2 && channelName.size() <= 16))
        return (0);
    return (1);
}

void Server::passCommand(int i)
{

    std::string key = this->_clients[i].getMessage().getToken();
    key = trimFunc(key);
    
    std::string nickname = this->_clients[i].getNickName();
    if (nickname.empty())
        nickname = "*";
        
    if (key.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(nickname, "PASS"));

    if (this->_clients[i].getAuthenticate())
        return this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName()));

    if (key != this->_passWord)
        return this->_clients[i].sendMsg(ERR_PASSWDMISMATCH(nickname));

    this->_clients[i].setPass(true);
    this->_clients[i].sendMsg(RPL_VALIDPASS());
}

void Server::nickCommand(int i)
{
    std::string params = this->_clients[i].getMessage().getToken();
    params = trimFunc(params);
    std::string nickname = this->_clients[i].getNickName();
    if (nickname.empty())
        nickname = "*";

    if (params.empty())
        return this->_clients[i].sendMsg(ERR_NONICKNAMEGIVEN(nickname));
    if (this->_clients[i].getPass() == false)
        return this->_clients[i].sendMsg(ERR_NOTREGISTERED(nickname));
    if (!this->checkNickName(params))
        return this->_clients[i].sendMsg(ERR_ERRONEUSNICKNAME(nickname, this->_clients[i].getMessage().getToken()));
    if (this->getClientByNickName(params) != NULL)
        return this->_clients[i].sendMsg(ERR_NICKNAMEINUSE(nickname, this->_clients[i].getMessage().getToken()));
    else
    {
        std::string msg = CHANGENICK(nickname , _clients[i].getUserName(), _clients[i].getIP(), params);
        std::map<std::string, Channel> ::iterator it = this->_channels.begin();
        std::string oldnickname = this->_clients[i].getNickName();
        this->_clients[i].setNickName(params);
        for(; it != _channels.end(); ++it)
        {
            if (it->second._clients.find(oldnickname) != it->second._clients.end()) {
                it->second._clients.erase(oldnickname);
                it->second._clients.insert(std::make_pair(params, &this->_clients[i]));
                
                it->second.broadcastMessage(msg);
            }
        }
        this->_clients[i].setNickName(params);
        return this->_clients[i].sendMsg(RPL_VALIDNICK());
    }
}

void Server::userCommand(int i)
{
    std::string params;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "USER")));
        
    if (this->_clients[i].getAuthenticate()) // Check if the user is already registered
        return (this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName())));
    std::vector<std::string> argsVec = splitString(params, ' ');
    
    if (argsVec.empty() || argsVec.size() != 4)
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "USER")));
    
    if (!checkUserName(argsVec[0])) // check param if valide.
        return this->_clients[i].sendMsg(ERR_ERRONEUSUSERNAME(this->_clients[0].getNickName(), params));
    
    if (this->_clients[i].getNickName().empty() || this->_clients[i].getPass() == false)
        return this->_clients[i].sendMsg(ERR_NOTREGISTERED(this->_clients[i].getNickName()));
        
    this->_clients[i].setUserName(argsVec[0]);
    this->_clients[i].setAuthenticate(true);

    this->_clients[i].sendMsg(RPL_WELCOME(this->_clients[i].getNickName(), this->_clients[i].getUserName(), this->_clients[i].getIP()));
    this->_clients[i].sendMsg(RPL_YOURHOST(this->_clients[i].getNickName()));
    this->_clients[i].sendMsg(RPL_CREATED(this->_clients[i].getNickName(), this->_clients[i].getTime()));
    
}

std::vector<std::pair<std::string, std::string> > parseChannels(std::string params) {
    std::vector<std::pair<std::string, std::string> > channels;
    size_t la = 0;

    // parse channel names
    for (size_t i = 0; i < params.size(); i++) {
        if (isspace(params[i])) {
            la = i;  // Remember position of first space
            break;
        }

        if (params[i] == '#') {  // Start of a channel name
            std::string name = "";
            while (i < params.size() && params[i] != ',' && !isspace(params[i])) {
                name += params[i];
                i++;
            }
            channels.push_back(std::make_pair(name, ""));
            if (params[i] == ',') {  // continue to next name
                continue;
            } else {
                break;
            }
        }
    }

    // Skip whitespace between names and keys
    while (la < params.size() && isspace(params[la])) {
        la++;
    }

    size_t idx = 0;

    // Parse keys
    for (size_t i = la; i < params.size(); i++) {
        std::string key = "";
        while (i < params.size() && params[i] != ',') {
            key += params[i];
            i++;
        }

        // Assign the parsed key to the appropriate channel, if it exists
        if (idx < channels.size()) {
            channels[idx++].second = key;
        } else {
            break;
        }
    }

    return channels;
}


void Server::joinCommand(int i)
{
    std::string params = this->_clients[i].getMessage().getToken();
    if (params.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "JOIN"));
    std::vector<std::pair<std::string, std::string> > channels = parseChannels(params);
    for (size_t j = 0; j < channels.size(); j++) {

        // std::vector<std::string> argsVec = splitString(params, ' ');
        // if (argsVec.empty() || argsVec.size() > 2)
        //     return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "JOIN"));

        std::string channelname = channels[j].first;
        std::string key = channels[j].second;

        if (!isValidChannelName(channelname))
            return this->_clients[i].sendMsg(ERR_BADCHANMASK(this->_clients[i].getNickName(), channelname));

        if (this->_clients[i].getnbrChannels() >= LIMITCHANNELS)    // Check if client is over the channel limit
            return this->_clients[i].sendMsg(ERR_TOOMANYCHANNELS(this->_clients[i].getNickName(), channelname));
        
        if (findChannelName(channelname))
        {

            Channel &channel = this->_channels[channelname]; 

            if (!key.empty() && channel._passWord != key)
                return this->_clients[i].sendMsg(ERR_BADCHANNELKEY(this->_clients[i].getNickName(), key));

            if (is_memberInChannel(channelname, this->_clients[i]))
                return _clients[i].sendMsg(ERR_USERONCHANNEL(this->_clients[i].getNickName(), channelname));

            // userlimit nbr  defined 
            if (channel.getMode('l') && channel._clients.size() >= (size_t)channel.getUserlimit())
                return this->_clients[i].sendMsg(ERR_CHANNELISFULL(this->_clients[i].getNickName(), channelname));
            //invite only mode  
            if (channel.getMode('i') && !channel.isInvitee(this->_clients[i]))// Invite-only mode check
                return this->_clients[i].sendMsg(ERR_INVITEONLYCHAN(this->_clients[i].getNickName(), channelname));
            // required key to join the channel.
            if (channel.getMode('k') && (key.empty() || key != channel.getpassWord()))
                return this->_clients[i].sendMsg(ERR_BADCHANNELKEY(this->_clients[i].getNickName(), channelname));
        }
        else
        {
            std::cout << channelname << std::endl;
            this->createChannel(channelname, key);
        }
        this->_channels[channelname].addClient(this->_clients[i]);
        

        if (this->_channels[channelname].isInvitee(this->_clients[i]))
        {
            std::vector<std::string>& inviteeList = this->_channels[channelname]._invitee;
            std::vector<std::string>::iterator it = std::find(inviteeList.begin(), inviteeList.end(), this->_clients[i].getNickName());

            if (it != inviteeList.end())
                inviteeList.erase(it);
        }
    }
}

void Server::partCommand(int i)
{
    std::string params, reason;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PART"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PART"));

    std::string channelname = argsVec[0];
    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));
    if (!is_memberInChannel(channelname, this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelname));

    if (argsVec.size() > 1) //  reason provided
        reason = params.substr(channelname.size() + 1); 
    else // No reason provided
        reason = "No reason provided";
    
    if (!reason.empty() && reason[0] != ':')
        reason = ":" + reason; 

    std::string partMessage = ":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PART " + channelname + " " + reason + "\r\n";
    this->_channels[channelname].broadcastMessage(partMessage);
    this->_channels[channelname].removeClient(this->_clients[i], PART);
    
}

void Server::kickCommand(int i)
{
    std::string params, reason;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "KICK"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.size() < 2)
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "KICK"));

    std::string &channelname = argsVec[0];
    std::string &kickeduser = argsVec[1];

    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));

    if (!this->_channels[channelname].hasPermission(_clients[i]))
        return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));

    Client *cl = getClientByNickName(kickeduser); 
    if (cl == NULL)
        return this->_clients[i].sendMsg(ERR_NOSUCHNICK(kickeduser));

    if (!is_memberInChannel(channelname, *cl))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(kickeduser, channelname));

    if (argsVec.size() > 2)  // reason is provided
        reason = params.substr(channelname.size() + kickeduser.size() + 2 ); 
    else
        reason = "No reason provided";
       
    if (!reason.empty() && reason[0] != ':')
        reason = ":" + reason; 

    this->_channels[channelname].broadcastMessage( RPL_KICK(_clients[i].getNickName(),_clients[i].getUserName(),_clients[i].getIP(),channelname,kickeduser,reason));
    this->_channels[channelname].removeClient(*cl, KICK);
}


void Server::topicCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "TOPIC"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "TOPIC"));

    if (!findChannelName(argsVec[0]))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));

    if (!is_memberInChannel(argsVec[0], this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));

    if (this->_channels[argsVec[0]].getMode('t') == true)
    {
        if (!this->_channels[argsVec[0]].hasPermission(_clients[i]))
            return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), argsVec[0]));
    }

    if (argsVec.size() == 1)// display current topic
    {
        if (this->_channels[argsVec[0]].getTopic().empty())
            return this->_clients[i].sendMsg(RPL_NOTOPIC(this->_clients[i].getNickName(), argsVec[0]));
        else
            return this->_clients[i].sendMsg(RPL_TOPIC(this->_clients[i].getNickName(), argsVec[0], this->_channels[argsVec[0]].getTopic()));
    }
    // topic is provided
    if (argsVec.size() > 1)
    {
        std::string newTopic = params.substr(argsVec[0].size() + 1); // +1 to skip the space after the channel name
        if (!newTopic.empty() && newTopic[0] == ':')
            newTopic = newTopic.substr(1); // rm ':' from topic

        this->_channels[argsVec[0]].setTopic(newTopic, this->_clients[i]);
    }
}

void Server::privmsgCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PRIVMSG"));
    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.size() < 2)
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PRIVMSG"));

    std::string target = argsVec[0];
    if (target.at(0) == '#') // target channel.
    {
        if (findChannelName(target) == false)
            return this->_clients[i].sendMsg(ERR_NOSUCHSERVER(this->_clients[i].getNickName()));
        if (!is_memberInChannel(target, this->_clients[i]))
            return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), target));
        
        std::string msg = params.substr(target.size() + 1) ;
        this->_channels[target].broadcastMessage(":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PRIVMSG " + target + " " + msg + "\r\n");
    }
    else // target client.
    {

        Client &sender = this->_clients[i];       // sender .
        Client *cl = getClientByNickName(target); // reciever client .
        if (cl == NULL)
            return this->_clients[i].sendMsg(ERR_NOSUCHNICK(target));
        cl->sendMsg(":" + sender.getNickName() + "!~" + sender.getUserName() + "@" + sender.getIP() + " PRIVMSG " + target + "" + params.substr(argsVec[0].size()) + "\r\n");
    }
}

void Server::listCommand(int i)
{
    std::string params = this->_clients[i].getMessage().getToken();
    std::vector<std::string> argsVec = splitString(params, ' ');

    if (argsVec.empty())
    {
        for (std::map<std::string, Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
        {
            const std::string &channelName = it->first;
            const Channel &channel = it->second;

            // Send = channel name : nbr of users in the channel : topic
            this->_clients[i].sendMsg(RPL_LIST(this->_clients[i].getNickName(), channelName, channel._clients.size(), channel.getTopic()));
        }
    }
    else
    {
        if (!findChannelName(argsVec[0]))
            return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        Channel &channel = this->_channels[argsVec[0]];
        this->_clients[i].sendMsg(RPL_LIST(this->_clients[i].getNickName(), argsVec[0], channel._clients.size(), channel.getTopic()));
    }
    this->_clients[i].sendMsg(RPL_LISTEND(this->_clients[i].getNickName()));
}


void Server::inviteCommand(int i)
{
    std::string params;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "INVITE"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.size() < 2)
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "INVITE"));

    std::string &inviteduser = argsVec[0]; 
    std::string &channelname = argsVec[1]; 

    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));

    if (!is_memberInChannel(channelname, this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelname));

    if (this->_channels[channelname].getMode('i') && !this->_channels[channelname].hasPermission(_clients[i]))
        return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));

    Client *cl = getClientByNickName(inviteduser);
    if (cl == NULL)
        return this->_clients[i].sendMsg(ERR_NOSUCHNICK(inviteduser));

    if (is_memberInChannel(channelname, *cl))
        return this->_clients[i].sendMsg(ERR_USERONCHANNEL(inviteduser, channelname));

    this->_channels[channelname].addInvitee(*cl);

    // Notify the invited user about the invitation
    cl->sendMsg(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));

    // Send confirmation to the inviter
    this->_clients[i].sendMsg(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));


}


void Server::applyMode(const std::vector<std::string> &argsVec, int i)
{
    std::string channelName = argsVec[0];
    std::string mode = argsVec[1];
    Channel &channel = this->_channels[channelName];
    Client &client = this->_clients[i];
    Client *targetClient = NULL;

    if (!channel.hasPermission(client))
       return client.sendMsg(ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));

    bool signal = (mode[0] == '+');
    char modeType = mode[1];


    switch (modeType)
    {
    case 'i':
        channel.setInviteOnly(signal);
        channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " " + "i" + " :Invite-only mode " + (signal ? "enabled" : "disabled") + "\r\n");
        break;
    case 't':
        channel.setTopicRestricted(signal);
        channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " " + "t" + " :Topic restriction " + (signal ? "enabled" : "disabled") + "\r\n");
        break;
    case 'k':
        if (signal)
        {
            if (argsVec.size() < 3)
                return (client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE +k")));
            std::string key = argsVec[2];
            channel._mode.requiredKey = true;
            channel.setpassWord(key);
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " +k :Channel key set" + "\r\n");
        }
        else
        {
            channel._passWord.clear();
            channel._mode.requiredKey = false;
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  + " MODE " + channelName + " -k :Channel key removed" + "\r\n");
        }
        break;
    case 'l':
        if (signal)
        {
            if (argsVec.size() < 3)
                return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE +l"));

            int limit = std::stoi(argsVec[2]);
            if (limit <= 0)
                return client.sendMsg(ERR_SYNTAXERROR(client.getNickName(), "MODE +l"));
            channel.setUserLimit(limit);
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " +l :User limit set to " + std::to_string(limit) + "\r\n");
        }
        else
        {
            channel._mode.userLimit = false;
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  +" MODE " + channelName + " -l :User limit removed " + "\r\n");
        }
        break;
    case 'o':
        if (argsVec.size() < 3)
            return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), " MODE +o/-o"));

        targetClient = getClientByNickName(argsVec[2]);
        if (targetClient == NULL)
            return (client.sendMsg(ERR_NOSUCHNICK(argsVec[2])));

        if (signal)
        {
            channel._clients[targetClient->getNickName()]->setOperStatus(true);
            channel.refrechChannel(*targetClient);
        }
        else
        {
            channel._clients[targetClient->getNickName()]->setOperStatus(false);
            channel.refrechChannel(*targetClient);
        }
        channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  + " MODE " + channelName + " " + (signal ? "+o " : "-o ") +  argsVec[2] + "\r\n");
        // channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  + " MODE " + channelName + " " + "o" +
        //     " :Operator privileges " + (signal ? "granted to " : "removed from ") + argsVec[2] + "\r\n");
        break;
    default:
        client.sendMsg(ERR_UNKNOWNMODE(client.getNickName(), mode));
        break;
    }
}



void Server::modeCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "MODE")));
    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty() || argsVec.size() > 3)
        return (this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "MODE")));

    std::string &channelName = argsVec[0];
    if (!channelName.empty() && channelName.at(0) == '#')
    {
        if (!findChannelName(channelName))
            return (this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!is_memberInChannel(channelName, this->_clients[i]))
            return (this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!this->_channels[channelName].hasPermission(_clients[i]))
            return (this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelName)));

        if (argsVec.size() == 1) // display modes
        {
            std::string key = (_channels[channelName].getMode('k') ? _channels[channelName].getpassWord() : "");
            std::string userlimit = "";
            if(_channels[channelName].getMode('l'))
                 userlimit = intToString(_channels[channelName].getUserlimit());

            return (this->_clients[i].sendMsg(RPL_CHANNELMODEIS(this->_clients[i].getNickName(), _channels[channelName].getChannelName(), _channels[argsVec[0]].showModes(), key, userlimit) ));
        }
        std::string &mode = argsVec[1];
        if (mode.empty() || (mode.at(0) != '+' && mode.at(0) != '-')   )
            return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "MODE"));
        applyMode(argsVec, i);

    }
}
