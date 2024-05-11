/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:19:10 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/05/11 21:15:53 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <poll.h>
#include <map>
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
    ////ussef
    Channel findChannel(std::string channelName);
    Channel createChannel(std::string channelName);
    void  joinCommand();
    void  partCommand();
    void  kickCommand();
    void  privmsgCommand();
    void  noticeCommand();
    void  topicCommand();
    void  inviteCommand();
    void  quitCommand();
    void  modeCommand();

};

#endif
