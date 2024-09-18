/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:14 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/18 10:39:11 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Client.hpp"

Client::Client()
{
    this->_clientFdSocket = -1;
    this->_nbrchannels = 0;
    this->_authenticate = false;
    this->_pass = false;
    this->_userName = "";
    this->_nickName = "";
    this->_IP = "";
}

Client::Client(const Client& obj)
{
    *this = obj;
}

Client& Client::operator=(const Client& obj)
{
    if (this != &obj)
    {
        this->_clientFdSocket = obj._clientFdSocket;
        this->_authenticate = obj._authenticate;
        this->_pass = obj._pass;
        this->_userName = obj._userName;
        this->_nickName = obj._nickName;
        this->_IP = obj._IP;
        this->_nbrchannels = obj._nbrchannels;
        this->_msg = obj._msg;
    }
    return *this;
}

Client::~Client()
{
    this->_userName.clear();
    this->_nickName.clear();
    this->_IP.clear();
}



/////////////////////////////////////////////////////////////////



Client::Client(int clientFdSocket, bool authenticate) :  _authenticate(authenticate)
{

    this->_userName = "";
    this->_nickName = "";
    this->_IP = "";
    this->_pass = false;
    this->_clientFdSocket = clientFdSocket;
    this->_nbrchannels = 0;
}

int Client::getClientFdSocket() const
{
    return this->_clientFdSocket;
}

bool Client::getAuthenticate() const
{
    return this->_authenticate;
}

std::string Client::getUserName() const
{
    return this->_userName;
}

std::string Client::getNickName() const
{
    return this->_nickName;
}

std::string Client::getIP() const
{
    return this->_IP;
}

Message& Client::getMessage()
{
    return this->_msg;
}


void Client::setClientFdSocket(int fd)
{
    this->_clientFdSocket = fd;
}

void Client::setAuthenticate(bool au)
{
    this->_authenticate = au;
}


void Client::setUserName(std::string userName)
{
    this->_userName = userName;
}

void Client::setNickName(std::string nickName)
{
    this->_nickName = nickName;
}

void Client::setIP(std::string IP)
{
    this->_IP = IP;
}

void Client::setMessage(Message msg)
{
    this->_msg.myAppend(msg);
}

void Client::setPass(bool b)
{
    this->_pass = b;
}

void Client::disconnect()
{
//     if (this->_authenticate)
//     {
//         for (std::vector<Channel>::iterator it = this->_channels.begin(); it < this->_channels.end(); it++)
//             (*it).removeClient(this->_clientFdSocket);        
//     }
}

void  Client::sendMsg(std::string str)
{
    send(this->_clientFdSocket, str.c_str(), str.size(), 0);
}

bool Client::getPass() const
{
    return this->_pass;
}

int  Client::getnbrChannels()
{
    return(this->_nbrchannels);
}
void Client::setnbrChannels(char sign)
{
    if(sign == '+')
        this->_nbrchannels++; 
    else
        this->_nbrchannels--; 
}
 

std::string Client::getTime() const
{
    std::time_t currentTime = std::time(0);
    std::tm* localTime = std::localtime(&currentTime);
    char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return(std::string(buffer));
}

