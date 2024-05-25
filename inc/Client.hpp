/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:18:23 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/05/25 03:45:51 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include "Message.hpp"
#include "Channel.hpp"
#include <sys/types.h>
#include <sys/socket.h>
class Channel;

class Client
{
private:
    int 					_clientFdSocket;
    bool					_authenticate;
    bool                    _pass;
    std::string				_currentChannel;
    std::string				_userName;
    std::string				_nickName;
    std::string				_IP;
    std::vector<Channel>	_channels;
    Message					_msg;
public:
    Client();
    Client(const Client& obj);
    Client& operator=(const Client& obj);
    ~Client();

    // param constructor
    Client(int clientFdSocket, bool authenticate);
    
    // getters
    int getClientFdSocket() const;
    bool getAuthenticate() const;
    bool getPass() const;
    std::string getCurrentChannel() const;
    std::string getNickName() const;
    std::string getUserName() const;
    std::string getIP() const;
    Message& getMessage();
    size_t getChannelsize();
    
    // setters
    void setClientFdSocket(int fd);
    void setAuthenticate(bool au);
    void setCurrentChannel(std::string channelName);
    void setUserName(std::string userName);
    void setNickName(std::string nickName);
    void setIP(std::string IP);
    void setMessage(Message msg);
    void setPass(bool b);
    
    // utils
    void disconnect();
    void sendMsg(std::string str);                                    
};

#endif
