/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:16:50 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/08/08 14:02:27 by ybouchra         ###   ########.fr       */
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


typedef struct s_Mode
{
    bool inviteOnly;
    bool topicRestricted;
}t_Mode;

class Channel
{
friend class Server;
private:
    int						        _userLimit;
    std::string				        _channelName;
    std::string				        _passWord;
    std::string				        _topic;
    t_SetterCl                      _setterCl;
    t_Mode                          _mode;
    std::map<std::string, Client>	_clients;
    std::map<std::string, Client>	_operators;
    std::map<std::string, Client>	_invitees;
    std::map<std::string, Client>	_bannedClient;

public:
    Channel();
    // Channel(std::string channelName, std::string key);
    Channel& operator=(const Channel& obj);
    Channel(const Channel& obj);
    ~Channel();

    // param constructor

    // getters
    std::string getChannelName() const;
    std::string getpassWord() const;
    std::string getTopic() const;
    bool        getMode(char token) const;
    int         getUserlimit() const;
    std::string getTime() const;
    

    
    
    // setters
    void setChannelName(std::string newName);
    void setpassWord(std::string newpassWord);
    void setTopic(std::string newTopic, Client setter);
    void setUserLimit(int limit);
    void setInviteOnly(bool mode);
    void setTopicRestricted(bool mode);
    void setOperator(Client &cl, bool mode);

    void addClient(Client cli);
    void removeClient(Client cli);
    void addOperators(Client ope);
    bool hasPermission (Client cli);  
    void brodcastMessage(Client sender, std::string msg);
    bool isBannedFromChannel(Channel ch, Client cl);
    // applyMode();
};

#endif