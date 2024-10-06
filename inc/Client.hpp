/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:18:23 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/06 05:51:44 by ybouchra         ###   ########.fr       */
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
#include <poll.h>



class Channel;
class Client
{
private:
    int 					_clientFdSocket;
    int                     _nbrchannels;
    bool					_authenticate;
    bool                    _pass;
    bool                    _operStatus;
    std::string				_userName;
    std::string				_nickName;
    std::string				_IP;
    Message					_msg;
    std::string             stream;
    
public:
    Client();
    Client(int clientFdSocket, bool authenticate);
    Client(const Client& obj);
    Client& operator=(const Client& obj);
    ~Client();

    
    // getters
    int         getClientFdSocket() const;
    bool        getAuthenticate() const;
    bool        getPass() const;
    std::string getNickName() const;
    std::string getUserName() const;
    std::string getIP() const;
    Message&    getMessage();                  
    std::string getTime() const;
    int         getnbrChannels();



    
    
    // setters
    void setClientFdSocket(int fd);
    void setAuthenticate(bool au);
    void setUserName(std::string userName);
    void setNickName(std::string nickName);
    void setIP(std::string IP);
    void setMessage(Message msg);
    void setPass(bool newPass);
    void setnbrChannels(char sign);

    
    
    // utils

    void sendMsg(std::string str);
    void consume_message(const std::string& s);
    void writeMessageToSocket();
    
};

#endif
