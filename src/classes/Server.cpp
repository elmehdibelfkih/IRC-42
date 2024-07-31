/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:33 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/07/31 23:48:04 by ybouchra         ###   ########.fr       */
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
            switch (this->_clients[i].getMessage().getCommand())
                {
                    case(JOIN):
                        joinCommand(i);
                        break;
                    case(LIST):
                        listCommand(i);
                        break;
                    case(PART):
                        partCommand(i);
                        break;
                    case(TOPIC):
                        topicCommand(i);
                        break;
                    case(PRIVMSG):
                        privmsgCommand(i);
                        break;
                    case(NOTICE):
                        noticeCommand(i);
                        break;     
                    case(PING):
                        pingCommand(i);
                        break;     
                    case(INVITE):
                        inviteCommand(i);
                        break;
                    case(KICK):
                        kickCommand(i);
                        break;
                    case(MODE):
                        modeCommand(i);
                        break;
                    // case(QUIT):
                    //     quitCommand(i);
                    //     break;

                    
                }
           
        }
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
        if (this->_clients[i].getMessage().getToken().empty())
            this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PASS"));
        else if (this->_clients[i].getPass() == true)
            this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName()));
        else if (this->_clients[i].getMessage().getToken() == this->_passWord)
            this->_clients[i].setPass(true);
        else
            this->_clients[i].sendMsg(ERR_PASSWDMISMATCH(this->_clients[i].getNickName()));
    }
    else
         this->_clients[i].sendMsg(ERR_NOTREGISTERED(this->_clients[i].getNickName()));
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

// void Server::userCommand(int i)
// {
//     if (this->_clients[i].getMessage().getCommand() == USER) 
//     {
//         if (this->_clients[i].getMessage().getToken().size() == 0)
//             this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS((std::string)"x",(std::string)"user"));
//         // else if (this->_clients[i].getUserName().size() == 0)
//         //     this->_clients[i].sendMsg(ERR_ALREADYREGISTERED((std::string)"x"));
//         else
//         {
//             this->_clients[i].setUserName("*****");
//             this->_clients[i].setAuthenticate(true);
//         }
            
//         // else if ()
            
//     }
//     else
//          this->_clients[i].sendMsg(ERR_NOTREGISTERED(this->_clients[i].getNickName()));
// }
void Server::userCommand(int i)
{
    std::string params;
    if (this->_clients[i].getMessage().getCommand() == USER) 
    {
        params =  this->_clients[i].getMessage().getToken();
        if (params.empty())
        {
            this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"USER"));
            return;
        }
        // if (this->_clients[i].getUserName().size() == 0)
        // {
        //     this->_clients[i].sendMsg(ERR_ALREADYREGISTERED((std::string)"x"));
        //     return;
        // }
        if (this->_clients[i].getAuthenticate())
        {
            this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName()));
            return;
        }
        // if(params.size() < 2 || params.size() > 12)
        // {
        //     this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "USER")); 
        //         return;
        // }

            this->_clients[i].setUserName(params);
            this->_clients[i].setAuthenticate(true);
    }
}



void Server::createChannel(std::string ch, std::string key)
{
    
    Channel tmp_ch;
    this->_channels.insert(std::pair< std::string, Channel>(ch, tmp_ch));
            this->_channels[ch].setChannelName(ch);
            this->_channels[ch].setpassWord(key);
            this->_channels[ch].setMode("=");
            this->_channels[ch]._topic = "";
            this->_channels[ch]._setterCl.nickName = "";
            this->_channels[ch]._setterCl.time = this->_channels[ch].getTime();
    
}
bool Server::findChannelName(std::string channelName)
{
    if(this->_channels.empty() || channelName.empty())
        return(false);
    std::map<std::string, Channel> ::iterator it = this->_channels.lower_bound(channelName);
    if(it != this->_channels.end() &&  it->first == channelName)
        return true;
    return(false);
}

bool Server::is_memberInChannel(std::string channelName, Client cl)
{
        std::map<std::string ,Client>::iterator it = this->_channels[channelName]._clients.lower_bound(cl.getNickName());
        if(it != _channels[channelName]._clients.end() && it->first == cl.getNickName())
            return(true);
        return(false);
}


bool Server::isValidChannelKey( std::string key)
{
        if( key.empty())
                return(true);
        if(key.size() < 4 || key.size() >= 32)
            return(false);
        for(size_t i = 0; key.size() > i ;i++)
        {
            if(key[i] == ' ' || (key.at(i) >= 9 && key.at(i) <= 13))
                return(false);
        }
        return(true);
            
}
bool Server::isValidChannelName(std::string channelName)
{
    if(channelName.empty())
        return(0);
    if(channelName.at(0) != '#' && channelName.at(0) != '&')
        return(0);
    if(!(channelName.size() >= 4 && channelName.size()<= 16))
        return(0);
    return(1);         
}

std::vector<std::string> Server::splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream iss(str);
    std::string key;
    while (std::getline(iss, key, delimiter)) {
        result.push_back(key);
    }
    return result;
}
void Server::joinCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
    
  
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"JOIN")); 
        return;
    }    
    argsVec = splitString(params, ' ');  
    if(argsVec.empty() || argsVec.size() > 2)
    {
      this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"JOIN")); 
        return;
    }
        
            if(!isValidChannelName(argsVec[0]))
            {
                this->_clients[i].sendMsg(ERR_BADCHANMASK(this->_clients[i].getNickName(), argsVec[0])); //channel name is not a valid.
                return;
            }      
            if(this->_clients[i].getChannelsize() >= LIMITCHANNELS) // the client has joined their maximum number of channels.
            {
                this->_clients[i].sendMsg(ERR_TOOMANYCHANNELS(this->_clients[i].getNickName(), argsVec[0])); 
                return;
            }
            if(findChannelName(argsVec[0]) == true)
                {
                    if(is_memberInChannel(argsVec[0], this->_clients[i]))
                    {
                            this->_clients[i].sendMsg(ERR_USERONCHANNEL(this->_clients[i].getNickName(), _clients[i].getNickName())); 
                            return;
                    }
                    if(this->_channels[argsVec[0]]._clients.size() >= (size_t)this->_channels[argsVec[0]].getUserlimit())  //Channel is FULL
                    {
                        this->_clients[i].sendMsg(ERR_CHANNELISFULL(this->_clients[i].getNickName(),argsVec[0])); 
                        return;   
                    }
                    if(this->_channels[argsVec[0]].getMode() == "+b")
                    {
                        if(this->_channels[argsVec[0]].isBannedFromChannel(_channels[argsVec[0]], _clients[i]))
                        {
                            this->_clients[i].sendMsg(ERR_BANNEDFROMCHAN(this->_clients[i].getNickName(),argsVec[0])); 
                            return;
                        }
                    }
                    if(this->_channels[argsVec[0]].getMode() == "+i")
                    {
                        this->_clients[i].sendMsg(ERR_BANNEDFROMCHAN(this->_clients[i].getNickName(),argsVec[0])); 
                        return;
                    }
                        
                        this->_channels[argsVec[0]].addClient(this->_clients[i]);
                }
            else
            {
                if(argsVec.size() == 2)
                {
                    if(!isValidChannelKey(argsVec[1]))
                    {
                        this->_clients[i].sendMsg(ERR_BADCHANNELKEY(this->_clients[i].getNickName(), argsVec[1])); //channel key is not a valid.
                            return;
                    }
                    this->createChannel(argsVec[0], argsVec[1]);
                }
                else
                    this->createChannel(argsVec[0], "");
                this->_channels[argsVec[0]].addClient(this->_clients[i]);
            }
        argsVec.clear();          
}

void Server::partCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string reason;
    std::string params = this->_clients[i].getMessage().getToken();
  
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"PART")); 
        return;
    }  
    argsVec = splitString(params, ' ');
    if( argsVec.empty())
    {
      this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"PART")); 
        return;
    }
    
        if (!findChannelName(argsVec[0])) {
            this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }
        if (!is_memberInChannel(argsVec[0], this->_clients[i]))
        {
            this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }
        this->_channels[argsVec[0]].removeClient(this->_clients[i]);
        if(argsVec[1].empty())
            this->_clients[i].sendMsg(RPL_SUCCESS(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP() , "" ));
        else
        {
            reason = params.substr(argsVec[0].size());
            this->_clients[i].sendMsg(RPL_SUCCESS(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP() , reason ));
        }
        argsVec.clear();
        reason.clear();
        
    }
void Server::topicCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
  
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"TOPIC")); 
        return;
    }    
    argsVec = splitString(params, ' ');
    if(argsVec.empty() || argsVec.size() > 2)
    {
      this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"TOPIC")); 
        return;
    }
    if (!findChannelName(argsVec[0])) {
        this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        return;
    }
    if (!is_memberInChannel(argsVec[0], this->_clients[i])) {
        this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        return;
    }
    if(this->_channels[argsVec[0]].getMode() == "+t")
    {
        if(!this->_channels[argsVec[0]].hasPermission(_clients[i]))
        {
            this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }
    }
    if(argsVec.size() == 1)
        this->_clients[i].sendMsg( this->_channels[argsVec[0]].getTopic() + "\r\n");
    else if(argsVec[1] == ":" )
     this->_channels[argsVec[0]].setTopic("", this->_clients[i]);
    else if( argsVec[1].at(0) == ':' && argsVec[1].size() > 2)
        this->_channels[argsVec[0]].setTopic(argsVec[1].substr(1), this->_clients[i]);
    else
    {
        this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"TOPIC"));
            return;
            
    }    
    argsVec.clear();
 
}


 void Server::sendingOper(Client sender, Client receiver, std::string msg)
{
    if(sender.getNickName() != receiver.getNickName())
{
        if (receiver.getStatus() == true)
           sender.sendMsg(RPL_AWAY(sender.getNickName(), msg));
        else
        receiver.sendMsg( sender.getNickName() + ": [" + msg + "] " + sender.getTime());
}
        
} 

void Server::privmsgCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"PRIVATE MESSAGE")); 
        return;
    }    

    argsVec = splitString(params, ':');
    if(argsVec.empty() || argsVec.size() < 2)
    {
        this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"PRIVATE MESSAGE")); 
            return;
    }
    if(!argsVec[0].empty())
        argsVec[0].erase( argsVec[0].size() - 1);
    if (argsVec[0].at(0) == '#' ) 
        {
            if(findChannelName(argsVec[0]) == false)
            {
                this->_clients[i].sendMsg(ERR_NOSUCHSERVER(this->_clients[i].getNickName()));
                    return;
            }
            if (!is_memberInChannel(argsVec[0], this->_clients[i]))
            {
                this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
                return;
            }
            if(this->_channels[argsVec[0]].getMode() == "+b")
            {
                if(this->_channels[argsVec[0]].isBannedFromChannel(this->_channels[argsVec[0]], this->_clients[i]) == true)
                    {
                        this->_clients[i].sendMsg(ERR_BANNEDFROMCHAN(this->_clients[i].getNickName(),argsVec[0])); 
                        return;
                    }
            }
            if(this->_channels[argsVec[0]].getMode() == "+m")
            {
                if(!this->_channels[argsVec[0]].hasPermission(_clients[i]))
                {
                    this->_clients[i].sendMsg(ERR_CANNOTSENDTOCHAN(this->_clients[i].getNickName(), argsVec[0]));
                    return;
                }
            }
                this->_channels[argsVec[0]].brodcastMessage(this->_clients[i], argsVec[1]);
        }    
    else if(argsVec[0].at(0) != '#')
        {
            Client *cl; 
            cl = getClientByNickName(argsVec[0]);
            if(cl != NULL )
            {
                this->sendingOper( this->_clients[i], *cl, argsVec[1]);
                return;
            }
            else
            {
                this->_clients[i].sendMsg(ERR_NOSUCHNICK( argsVec[0]));
                return;
            }
        }
    }
void Server::noticeCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
  
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"NOTICE")); 
        return;
    }    
    argsVec = splitString(params, ':');
    if(argsVec.empty() || argsVec.size() < 2 )
    {
        this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"NOTICE")); 
            return;
    }
    if(!argsVec[0].empty())
        argsVec[0].erase( argsVec[0].size() - 1);
    if (argsVec[0].at(0) == '#' ) 
        {
            if(findChannelName(argsVec[0]) == false)
                    return;
            if (!is_memberInChannel(argsVec[0], this->_clients[i]))
                return;
            if(this->_channels[argsVec[0]].getMode() == "+b" || this->_channels[argsVec[0]].getMode() == "+m" )
            {
                if(this->_channels[argsVec[0]].isBannedFromChannel(this->_channels[argsVec[0]], this->_clients[i]) == true)
                    return;
                 if(!this->_channels[argsVec[0]].hasPermission(_clients[i]))
                    return;
            }
            this->_channels[argsVec[0]].brodcastMessage(this->_clients[i], argsVec[1]);
        }    
    else if(argsVec[0].at(0) != '#')
        {
            Client *cl; 
            cl = getClientByNickName(argsVec[0]);
            if(cl != NULL )
            {
                this->sendingOper( this->_clients[i], *cl, argsVec[1]);
                return;
            }
        }
    }

void Server::pingCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"PING")); 
        return;
    }
    argsVec = splitString(params, ':');
    if(argsVec.empty() ||  argsVec.size() != 2)
    {
        this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "PING")); 
            return;
    }
    if( argsVec[1].empty())
    {
        this->_clients[i].sendMsg(ERR_NOORIGIN(this->_clients[i].getNickName()));
        return ;
    }
    
        this->_clients[i].sendMsg("PONG :" +  argsVec[1] );

    }

void Server::listCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"LIST")); 
        return;
    }
    argsVec = splitString(params, ' ');
    if(argsVec.empty() ||  argsVec.size() != 1)
    {
        this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "LIST")); 
            return;
    }
     if(!findChannelName(argsVec[0]))
     {
        this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
                    return;
     }
    if (!is_memberInChannel(argsVec[0], this->_clients[i]))
    {
        this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        return;
    }


    std::map<std::string, Client>::iterator it = this->_channels[argsVec[0]]._clients.begin(); 
    for( ; it != this->_channels[argsVec[0]]._clients.end(); ++it)
    this->_clients[i].sendMsg(it->second.getNickName() + "\n");
}



void Server::inviteCommand(int i)
{
    std::vector<std::string>argsVec;
    std::string params = this->_clients[i].getMessage().getToken();
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "INVITE")); 
        return;
    }
    argsVec = splitString(params, ' ');
    if(argsVec.empty() ||  argsVec.size() != 2)
    {
        this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "INVITE")); 
            return;
    }
    if (!findChannelName(argsVec[1])) {
        this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        return;
    }
    if (!is_memberInChannel(argsVec[1], this->_clients[i]))
    {
        this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        return;
    }
    if(this->_channels[argsVec[1]].getMode() == "+t")
    {
        if(!this->_channels[argsVec[1]].hasPermission(_clients[i]))
        {
            this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }
    }
      Client *cl = getClientByNickName(argsVec[0]); 
            if(cl == NULL )
            {
                this->_clients[i].sendMsg(ERR_NOSUCHNICK(argsVec[0]));
                return;
            }
    if (is_memberInChannel(argsVec[1], *cl))
    {
        this->_clients[i].sendMsg(ERR_USERONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        return;
    }
    
    this->_channels[argsVec[1]].addClient(*cl);
    this->_clients[i].sendMsg(RPL_INVITING(this->_clients[i].getNickName(), argsVec[0], argsVec[1]));
    }

void Server::kickCommand(int i)
{
  std::vector<std::string>argsVec;
    std::string reason;
    std::string params = this->_clients[i].getMessage().getToken();
  
    if(params.empty())
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"KICK")); 
        return;
    }  
    argsVec = splitString(params, ' ');
    if( argsVec.empty() || argsVec.size() < 2)
    {
      this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"KICK")); 
        return;
    }
    
        if (!findChannelName(argsVec[0])) {
            this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }
        if(!this->_channels[argsVec[0]].hasPermission(_clients[i]))
        {
            this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }

        Client *cl = getClientByNickName(argsVec[1]); 
        if(cl == NULL )
        {
            this->_clients[i].sendMsg(ERR_NOSUCHNICK(argsVec[0]));
            return;
        }
        if (is_memberInChannel(argsVec[0], *cl))
        {
            this->_clients[i].sendMsg(ERR_USERONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
            return;
        }
        if(argsVec.size() == 2)
        {
            
            cl->sendMsg(RPL_KICKEDUSER(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), argsVec[0], argsVec[1], ""));
            this->_channels[argsVec[0]].brodcastMessage(this->_clients[i], RPL_KICKEDUSER(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), argsVec[0], argsVec[1], reason));
            this->_channels[argsVec[0]].removeClient(*cl);
        }
        else
        {
            reason = params.substr(argsVec[0].size() + argsVec[1].size());
            cl->sendMsg(RPL_KICKEDUSER(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), argsVec[0], argsVec[1], reason));
            this->_channels[argsVec[0]].brodcastMessage(this->_clients[i], RPL_KICKEDUSER(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), argsVec[0], argsVec[1], reason));

        }
        argsVec.clear();
        reason.clear();
    
}
void Server::modeCommand(int i)
{
  std::vector<std::string>argsVec;
    std::string reason;
    std::string params = this->_clients[i].getMessage().getToken();
  
    if(params.empty())
    {
      this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"MODE")); 
        return;
    }  
    argsVec = splitString(params, ' ');
    if( argsVec.empty() || argsVec.size() < 2  || argsVec.size() > 3)
    {
        this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(),"MODE")); 
        return;
    }
      if (argsVec[0].at(0) == '#' ) 
        {

            if (!findChannelName(argsVec[0])) {
                this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
                return;
            }

                // applyMode();
                
  
            if(argsVec[1] == "+b" || argsVec[1] == "+t" || argsVec[1] == "+i" || argsVec[1] == "+k" || argsVec[1] == "+o")
                    this->_channels[argsVec[0]].setMode(argsVec[1]);
   
             this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(),"MODE")); 

        }
        else
        {
            Client *cl = getClientByNickName(argsVec[1]); 
            if(cl == NULL )
            {
                this->_clients[i].sendMsg(ERR_NOSUCHNICK(argsVec[0]));
                return;
            }
        if(argsVec.size() == 2)
        {
            
            cl->sendMsg(RPL_KICKEDUSER(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), argsVec[0], argsVec[1], ""));
            this->_channels[argsVec[0]].brodcastMessage(this->_clients[i], RPL_KICKEDUSER(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), argsVec[0], argsVec[1], reason));
            this->_channels[argsVec[0]].removeClient(*cl);
        }
              // else
                // replie   ....
                
        argsVec.clear();
        reason.clear();
    
}
}