/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:18:23 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/13 02:02:20 by ybouchra         ###   ########.fr       */
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
    int                     _nbrchannels;
    bool					_authenticate;
    bool                    _pass;
    std::string				_userName;
    std::string				_nickName;
    std::string				_IP;
    Message					_msg;
    
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
    void disconnect();
    void sendMsg(std::string str);
                           
};

#endif
