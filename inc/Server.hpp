/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:19:10 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/07/13 06:36:24 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <poll.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "replies.hpp"

class Server
{
private:
    uint16_t                        _port; // mehdi
    std::string                     _passWord; // mehdi
    std::vector<pollfd>             _fds; // mehdi
    std::map<int, Client>           _clients; // <==
    std::map<std::string, Channel>  _channels; // <== ++++++ 
    
public:
    Server();
    Server& operator=(const Server& obj);
    Server(const Server& obj);
    ~Server();

    Server(std::string port, std::string password);
    void startServer(); // mehdi
    bool authenticateUser(int i); // mehdi
    void handleClientConnection(); // mehdi
    void handleClientMessage(int i); // <==
    Client* getClientByNickName(std::string nick); // <==
    bool checkNickName(int i);
    bool checkUserName(int i);




    void    createChannel(std::string channelName, std::string key);
    bool    findChannelName(std::string channelName);
    bool    findClientByNick(std::string nickname);
    bool    is_memberInChannel(std::string channelName, Client cl);
    bool    isValidChannelName(std::string channelName);
    bool    isValidChannelKey(std::vector<std::string> keys, int index);
    
    std::string     findOpional(std::vector<std::string>&vec,int index);
    std::vector<std::string>    splitString(const std::string& str, char delimiter);

    
    void    passCommand(int i);
    void    nickCommand(int i);
    void    userCommand(int i);
    void    joinCommand(int i);
    void    partCommand(int i);
    void    topicCommand(int i);
    void    privmsgCommand(int i);
    // void  kickCommand(int i);
    // void  noticeCommand(int i);
    // void  inviteCommand(int i);
    // void  quitCommand(int i);
    // void  modeCommand(int i);
};

#endif
