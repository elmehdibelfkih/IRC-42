/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:33 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/05/20 09:18:33 by ybouchra         ###   ########.fr       */
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
        // if (this->authenticateUser(i))
        // {   
            switch (this->_clients[i].getMessage().getCommand())
                {
                    case(JOIN):
                        joinCommand(i);
                        break;
                    // case(PART):
                    //     partCommand(i);
                    //     break;
                    // case(KICK):
                    //     kickCommand(i);
                    //     break;
                    // case(PRIVMSG):
                    //     privmsgCommand(i);
                    //     break;
                    // case(NOTICE):
                    //     noticeCommand(i);
                    //     break;     
                    // case(TOPIC):
                    //     topicCommand(i);
                    //     break;
                    // case(INVITE):
                    //     inviteCommand(i);
                    //     break;
                    // case(QUIT):
                    //     quitCommand(i);
                    //     break;
                    // case(MODE):
                    //     modeCommand(i);
                    //     break;
                }
           
        // }
        this->_clients[i].getMessage().clearBuffer();
    }
}

bool Server::authenticateUser(int i)
{
    if (this->_clients[i].getAuthenticate())
        return true;
    else if (!this->_clients[i].getPass() || this->_clients[i].getMessage().getCommand() == PASS)
        this->passCommand(i);
    else if (this->_clients[i].getNickName().size() == 0 || this->_clients[i].getMessage().getCommand() == NICK)
        this->nickCommand(i);
    else if (this->_clients[i].getUserName().size() == 0 || this->_clients[i].getMessage().getCommand() == USER)
        this->userCommand(i);
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
    if (*(this->_clients[i].getMessage().getToken().begin()) == ':' || *(this->_clients[i].getMessage().getToken().begin()) == '$')
        return false;
    this->_clients[i].setNickName(this->_clients[i].getMessage().getToken());
    return true;
}

bool Server::checkUserName(int i)
{
    (void)i;
    return true;
}

void Server::passCommand(int i)
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

void Server::nickCommand(int i)
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

void Server::userCommand(int i)
{
    if (this->_clients[i].getMessage().getCommand() == USER)
    {
        if (this->_clients[i].getMessage().getToken().size() == 0)
            this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS((std::string)"x",(std::string)"user"));
        // else if (this->_clients[i].getUserName().size() == 0)
        //     this->_clients[i].sendMsg(ERR_ALREADYREGISTERED((std::string)"x"));
        else
        {
            this->_clients[i].setUserName("*****");
            this->_clients[i].setAuthenticate(true);
        }
            
        // else if ()
            
    }
    else
         this->_clients[i].sendMsg(ERR_NOTREGISTERED((std::string)"x"));
}


void Server::createChannel(std::string ch, std::string key)
{
    
    Channel tmp_ch;       
    this->_channels.insert(std::pair< std::string, Channel>(ch, tmp_ch));
            this->_channels[ch].setChannelName(ch);
            this->_channels[ch].setpassWord(key);
            // this->_channels[ch].set(ch);
            // this->_channels[ch].set(ch);

    
}

bool Server::is_existChannel(std::string channelName)
{
    if(this->_channels.empty() || channelName.empty())
        return(0);
    std::map<std::string, Channel> ::iterator it = this->_channels.lower_bound(channelName);
    if(it != this->_channels.end() &&  it->first == channelName)
        return 1;
    return(0);
}

bool Server::is_memberInChannel(std::string channelName, Client cl)
{
        std::map<std::string ,Client>::iterator it = this->_channels[channelName]._clients.lower_bound(cl.getNickName());
        if(it != _channels[channelName]._clients.end() && it->first == cl.getNickName())
            return(true);
        return(false);
}

std::string getChannelkey(std::vector<std::string>& channelkeys, int indexkey)
{

     std::string chKey = "";
    if (channelkeys.size() > 0 && !channelkeys[indexkey].empty())
        return(channelkeys[indexkey]);
        
    return(chKey);
    
}

void Server::joinCommand(int i)
{
    int indexkey = 0;
    std::string ch;
    std::vector<std::string>argsVec;
    std::vector<std::string>keysVec;
    std::stringstream iss(this->_clients[i].getMessage().getToken());
    
    while(std::getline(iss, ch, ' '))
        argsVec.push_back(ch);

      if(argsVec.size() == 0 || this->_clients[i].getMessage().getToken().size() == 0)
        {
            this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS((std::string)"x",(std::string)"user")); 
            return;
        }    
  
    if(argsVec.size() > 2 || argsVec[0].empty())
    {
        std::cerr << "Format ERROR\n";
        return;
    }
    if(argsVec.size() > 1)
    {
        std::stringstream keys(argsVec[1]); 
        std::string key;
        while( std::getline(keys, key, ','))
        keysVec.push_back(key);     
    }
            ch.clear();
            std::stringstream iss1(argsVec[0]);
            while(std::getline(iss1, ch, ','))
                {
                    if(this->_clients[i].getChannelsize() == LIMITCHANNELS)
                    {
                        this->_clients[i].sendMsg(ERR_TOOMANYCHANNELS(this->_clients[i].getNickName(),ch)); 
                        return;
                    }  
                    if(!ch.empty() && (ch.at(0) == '#' || ch.at(0) == '&'))
                    {
                        
                        if(is_existChannel(ch))
                            {
                                if(is_memberInChannel(ch, this->_clients[i]))
                                    std::cout << "you have allready joined the channel\n";
                                else
                                {
                                        this->_channels[ch].addClient(this->_clients[i]);
                                    std::cout << "you have joined the channel\n";   
                                }
                            }
                        else
                        {   
                            this->createChannel(ch, getChannelkey(keysVec, indexkey));
                            this->_channels[ch].addClient(this->_clients[i]);
                            std::cout << " the channel " << ch << " was created and you are joined to the channel\n";
                        // sendmsg(":" + _client.getNickname() + "!" + _client.getUsername() + "@" + xxxxxxxxip + " JOIN " + channelName + "\r\n");
                        // sendmsg(":" + xxxxxxxxip + " MODE " + channelName + " " + this->_channelObj.getModes() + "\r\n");
                        // sendmsg(":" + xxxxxxxxip + " 353 " + _client.getNickname() + " = " + channelName + " :@" + this->_client.getNickname() + "\r\n");
                        // sendmsg(":" + xxxxxxxxip + " 366 " + _client.getNickname() + " " + channelName + " :End of /NAMES list.\r\n");
                       indexkey++;
                       }
                    } 
                    else
                    {
                        this->_clients[i].sendMsg(ERR_BADCHANMASK(ch)); //channel name is not a valid.
                        continue; 
                    }
                   
                }
                  
    std::map<std::string, Channel> ::iterator it = this->_channels.begin();
    for(; it != this->_channels.end(); it++)
        std::cout << "ChannelName: " << it->first << "****" << it->second.getpassWord() << std::endl;


 
 
   
    
}