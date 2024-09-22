/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:33 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/22 06:58:53 by ybouchra         ###   ########.fr       */
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
            if (clientFdSocket < 0)
                std::cerr << "accept() failed" << std::endl;
            else
            {
                std::string clientIP = inet_ntoa(clientAddr.sin_addr);
                std::cout << "New client connected: " << clientIP << std::endl;

                pollfd tmp;
                tmp.fd = clientFdSocket;
                tmp.events = POLLIN;

                this->_fds.push_back(tmp);
                Client Ctmp(clientFdSocket, false);
                Ctmp.setIP(clientIP);
                this->_clients[clientFdSocket] = Ctmp;
            }
        }

        // Check for data on client sockets
        for (size_t i = 1; i < this->_fds.size(); i++)
        {
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
                    // this->_clients[this->_fds[i].fd].disconnect();
                    if (this->_clients[this->_fds[i].fd].getAuthenticate())
                    {
                        for ( std::map<std::string, Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
                        (  *it).second.removeClient(_clients[this->_fds[i].fd]); 
                    }
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
            switch (this->_clients[i].getMessage().getCommand())
            {
            case PASS:case NICK:case USER:case PONG:case QUIT:
                break;
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
            default:
                this->_clients[i].sendMsg(ERR_UNKNOWNCOMMAND(_clients[i].getNickName()));
            }
        }

        std::cout << this->_clients[i].getMessage().getBuffer() << std::endl;
        this->_clients[i].getMessage().clearBuffer();
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
    // if (username.length() < 3 || username.length() > 9)
    //     return false;

    for (size_t i = 0; i < username.size(); ++i)
    {
        char c = username[i];
        if (!(isalpha(c)))
            return false;
    }
    return true;
}

bool Server::checkNickName(std::string nickname)
{

    // if (nickname.length() < 3 || nickname.length() > 9)
    //     return false;

    for (size_t i = 0; i < nickname.size(); ++i)
    {
        char c = nickname[i];
        if (!(isalpha(c)))
            return false;
    }
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
    std::map<std::string, Channel>::iterator it = this->_channels.lower_bound(channelName);  
    return (it != this->_channels.end() );

}

bool Server::is_memberInChannel(std::string &channelName, Client cl)
{
    std::map<std::string, Client>::iterator it = this->_channels[channelName]._clients.lower_bound(cl.getNickName());
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
    if (!(channelName.size() >= 4 && channelName.size() <= 16))
        return (0);
    return (1);
}

void Server::passCommand(int i)
{

    std::string nickname = this->_clients[i].getNickName();
    if (nickname.empty())
        nickname = "*";

    if (this->_clients[i].getMessage().getToken().empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(nickname, "PASS"));

    if (this->_clients[i].getAuthenticate())
        return this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName()));

    if (this->_clients[i].getMessage().getToken() != this->_passWord)
        return this->_clients[i].sendMsg(ERR_PASSWDMISMATCH(nickname));

    this->_clients[i].setPass(true);
    this->_clients[i].sendMsg(RPL_VALIDPASS());
}

void Server::nickCommand(int i)
{
    std::string nickname = this->_clients[i].getNickName();
    if (nickname.empty())
        nickname = "*";

    if (this->_clients[i].getMessage().getToken().empty())
        return this->_clients[i].sendMsg(ERR_NONICKNAMEGIVEN(nickname));
    if (this->_clients[i].getAuthenticate()) 
        return this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName()));
    if (this->_clients[i].getPass() == false)
        return this->_clients[i].sendMsg(ERR_NOTREGISTERED(nickname));
    if (!this->checkNickName(this->_clients[i].getMessage().getToken()))
        return this->_clients[i].sendMsg(ERR_ERRONEUSNICKNAME(nickname, this->_clients[i].getMessage().getToken()));
    if (this->getClientByNickName(this->_clients[i].getMessage().getToken()) != NULL)
        return this->_clients[i].sendMsg(ERR_NICKNAMEINUSE(nickname, this->_clients[i].getMessage().getToken()));
    else
    {
        this->_clients[i].setNickName(this->_clients[i].getMessage().getToken());
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

void Server::joinCommand(int i)
{
    std::string params = this->_clients[i].getMessage().getToken();
    if (params.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "JOIN"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty() || argsVec.size() > 2)
        return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "JOIN"));

    std::string &channelname = argsVec[0];
    std::string key = (argsVec.size() == 2) ? argsVec[1] : "";

    if (!isValidChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_BADCHANMASK(this->_clients[i].getNickName(), channelname));

    // Check if client is over the channel limit
    if (this->_clients[i].getnbrChannels() >= LIMITCHANNELS)
        return this->_clients[i].sendMsg(ERR_TOOMANYCHANNELS(this->_clients[i].getNickName(), channelname));
    
    if (findChannelName(channelname))
    {
        Channel &channel = this->_channels[channelname]; 
        if (is_memberInChannel(channelname, this->_clients[i]))
            return _clients[i].sendMsg(ERR_USERONCHANNEL(this->_clients[i].getNickName()));

        // userlimit nbr  defined 
        if (channel.getMode('l') && channel._clients.size() >= (size_t)channel.getUserlimit())
            return this->_clients[i].sendMsg(ERR_CHANNELISFULL(this->_clients[i].getNickName(), channelname));

        if (channel.getMode('i'))// Invite-only mode check
            return this->_clients[i].sendMsg(ERR_INVITEONLYCHAN(this->_clients[i].getNickName(), channelname));

        // required key to join the channel.
        if (channel.getMode('k') && (key.empty() || key != channel.getpassWord()))
            return this->_clients[i].sendMsg(ERR_BADCHANNELKEY(this->_clients[i].getNickName(), channelname));
    }
    else
    {
        if (!key.empty() && !isValidChannelKey(key))
            return this->_clients[i].sendMsg(ERR_BADCHANNELKEY(this->_clients[i].getNickName(), key));

        this->createChannel(channelname, key);                        // Create the channel
        this->_channels[channelname].addOperators(this->_clients[i]); // Add the client as an operator
    }
    this->_channels[channelname].addClient(this->_clients[i]);
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

    if (this->_channels[channelname].hasPermission(_clients[i]))
        this->_channels[channelname].removeOperators(this->_clients[i]);

    if (argsVec.size() > 1) //  reason provided
        reason = params.substr(channelname.size() + 1); 
    else // No reason provided
        reason = "No reason provided";
        
    this->_channels[channelname].broadcastMessage(RPL_PART(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), channelname, reason));
    this->_channels[channelname].removeClient(this->_clients[i]);

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
    
    this->_channels[channelname].broadcastMessage(RPL_KICKEDUSER(_clients[i].getNickName(),_clients[i].getUserName(),_clients[i].getIP(),channelname,kickeduser,reason));
    this->_channels[channelname].removeClient(*cl);
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

    //   mode 't' topicRestricted
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
        this->_channels[target].broadcastMessage(":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PRIVMSG " + target + "" + params.substr(argsVec[0].size()) + "\r\n");
    }
    else // target client.
    {

        Client &sender = this->_clients[i];       // sender .
        Client *cl = getClientByNickName(target); // reciever client .
        if (cl == NULL)
            return this->_clients[i].sendMsg(ERR_NOSUCHNICK(target));
        if (sender.getNickName() == cl->getNickName())
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

            // Send channel name : nbr of users in the channel : topic
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

// 
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

    if (this->_channels[channelname].getMode('i') && !this->_channels[channelname].hasPermission(_clients[i])) {
        return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));
    }

    Client *cl = getClientByNickName(inviteduser);
    if (cl == NULL)
        return this->_clients[i].sendMsg(ERR_NOSUCHNICK(inviteduser));
    
    if (is_memberInChannel(channelname, *cl))
        return this->_clients[i].sendMsg(ERR_USERONCHANNEL(inviteduser));

    this->_channels[channelname].addClient(*cl);
    this->_channels[channelname].broadcastMessage(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));
}


void Server::applyMode(const std::vector<std::string> &argsVec, int i)
{
    std::string channelName = argsVec[0];
    std::string mode = argsVec[1];
    Channel &channel = this->_channels[channelName];
    Client &client = this->_clients[i];
    Client *targetClient = NULL;

    if (!channel.hasPermission(client))
    {
        client.sendMsg(ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));
        return;
    }
    bool signal = (mode[0] == '+');
    char modeType = mode[1];

    switch (modeType)
    {
    case 'i':
        channel.setInviteOnly(signal);
        channel.broadcastMessage("MODE " + channelName + " " + mode + " :Invite-only mode " + (signal ? "enabled" : "disabled") + "\r\n");
        break;
    case 't':
        channel.setTopicRestricted(signal);
        channel.broadcastMessage("MODE " + channelName + " " + mode + " :Topic restriction " + (signal ? "enabled" : "disabled") + "\r\n");
        break;
    case 'k':
        if (signal)
        {
            if (argsVec.size() < 3)
                return (client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE +k")));
            std::string key = argsVec[2];
            channel._mode.requiredKey = true;
            channel.setpassWord(key);
            channel.broadcastMessage("MODE " + channelName + " +k :Channel key set" + "\r\n");
        }
        else
        {
            channel._passWord.clear();
            channel._mode.requiredKey = false;
            channel.broadcastMessage("MODE " + channelName + " -k :Channel key removed" + "\r\n");
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
            channel.broadcastMessage("MODE " + channelName + " +l :User limit set to " + std::to_string(limit) + "\r\n");
        }
        else
        {
            channel._mode.userLimit = false;
            channel.broadcastMessage("MODE " + channelName + " -l :User limit removed " + "\r\n");
        }
        break;
    case 'o':
        if (argsVec.size() < 3)
            return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE +o/-o"));

        targetClient = getClientByNickName(argsVec[2]);
        if (targetClient == NULL)
            return (client.sendMsg(ERR_NOSUCHNICK(argsVec[2])));

        if (signal)
            channel.addOperators(*targetClient);
        else
            channel.removeOperators(*targetClient);
        channel.broadcastMessage("MODE " + channelName + " " + mode + " :Operator privileges " + (signal ? "granted to " : "removed from ") + argsVec[2] + "\r\n");
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

        if (argsVec.size() == 1) // display status modes of the channel "+t +k +i"
        {
            std::string key = (_channels[channelName].getMode('k') ? _channels[channelName].getpassWord() : "");
            return (this->_clients[i].sendMsg(RPL_CHANNELMODEIS(this->_clients[i].getNickName(), _channels[channelName].getChannelName(), _channels[argsVec[0]].showModes(), key)));
        }
        std::string &mode = argsVec[1];
        if (mode.empty() || (mode.at(0) != '+' && mode.at(0) != '-'))
            return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "MODE"));

        applyMode(argsVec, i);

//         for (size_t j = 0; j < mode.size(); ++j) {
//             applyMode({channelName, std::string(1, mode[j])}, i);
// }
    }
}
