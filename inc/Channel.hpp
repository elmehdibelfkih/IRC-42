/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:16:50 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/05/25 06:01:01 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include "Client.hpp"
#include "replies.hpp"
class Client;

typedef struct s_SetterCl
{
    std::string nickName;
    std::string time;
}t_SetterCl;

class Channel
{
friend class Server;
private:
    int						        _userLimit;
    std::string				        _channelName;
    std::string				        _passWord;
    std::string				        _topic;
    std::string				        _mode;
    t_SetterCl                      _setterCl;
    std::map<std::string, Client>	_clients;
    std::map<std::string, Client>	_operators;
    std::map<std::string, Client>	_invitees;
    std::map<std::string, Client>	_bannedClient;
public:
    Channel();
    Channel& operator=(const Channel& obj);
    Channel(const Channel& obj);
    ~Channel();

    // param constructor

    // getters
    std::string getChannelName() const;
    std::string getpassWord() const;
    std::string getTopic() const;
    std::string getMode() const;
    int getUserlimit() const;
    std::string getTime() const;
    
    // setters
    void setChannelName(std::string newName);
    void setpassWord(std::string newpassWord);
    void setTopic(std::string newTopic, Client setter);
    void setMode(std::string newMode);


    void addClient(Client cli);
    void removeClient(Client cli);
    void addOperators(Client ope);

    void brodcastMessage(std::string message, Client sender);
    bool isBannedFromChannel(Channel ch, Client cl);
};

#endif