/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:19:10 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/03 18:33:25 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cctype>
#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "replies.hpp"
#include "IRC.hpp"
#include <fcntl.h>

class Server
{

private:
    uint16_t                        _port; 
    std::string                     _passWord;
    std::vector<pollfd>             _fds; 
    std::map<int, Client>           _clients;
    std::map<std::string, Channel>  _channels; 
    
public:
    Server();
    Server(std::string port, std::string password);
    Server& operator=(const Server& obj);
    Server(const Server& obj);
    ~Server();

    void                        startServer(); 
    bool                        authenticateUser(int i); 
    void                        handleClientConnection(); 
    void                        handleClientMessage(int i); 
    bool                        checkNickName(std::string nickname);
    bool                        checkUserName(std::string username);
 
    void                        applyModes(const std::string &modes, const std::vector<std::string> &argsVec, int clientIdx);

    void                        createChannel(std::string &channelName, std::string key);
    bool                        findChannelName(std::string &channelName);
    bool                        is_memberInChannel(std::string &channelName, Client cl);
    bool                        isValidChannelName(std::string &channelName);
    bool                        isValidChannelKey(std::string &keys);
    Client                      *getClientByNickName(std::string nick);

    void                        passCommand(int i);
    void                        nickCommand(int i);
    void                        userCommand(int i);
    void                        joinCommand(int i);
    void                        partCommand(int i);
    void                        topicCommand(int i);
    void                        privmsgCommand(int i);
    void                        listCommand(int i);
    void                        inviteCommand(int i);
    void                        kickCommand(int i);
    void                        modeCommand(int i);
};

#endif
