/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:16:50 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/05 00:04:13 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <algorithm>
#include <map>
#include "Client.hpp"
#include "replies.hpp"
class Client;

typedef struct s_SetterCl // For Topic 
{
    std::string nickName;
    std::string userName;
    std::string ip;
    std::string time;
}t_SetterCl;


typedef struct s_Mode
{
    bool inviteOnly;
    bool topicRestricted;
    bool userLimit;
    bool requiredKey;
}t_Mode;

class Channel
{
friend class Server;
private:
    std::string				        _channelName;
    std::string				        _passWord;
    std::string				        _topic;
    int						        _userLimit;
    t_Mode                          _mode;
    t_SetterCl                      _setterCl;
    std::map<std::string, Client*>	_clients;
    std::vector<std::string>    	_invitee;
    std::vector<std::string>        _operators;
    std::string                     _creationTime;
    
public:
    Channel();
    Channel(std::string channelName, std::string key);
    Channel& operator=(const Channel& obj);
    Channel(const Channel& obj);
    ~Channel();


    // getters
    std::string getChannelName() ;
    std::string getpassWord() const;
    std::string getTopic() const;
    bool        getMode(char token) const;
    int         getUserlimit() const;
    std::string showModes() const;
    std::string getTime() const;
    std::string getCreationTime() const;
    
    // setters
    void setChannelName(std::string newName);
    void setpassWord(std::string newpassWord);
    void setTopic(std::string newTopic, Client setter);
    void setUserLimit(int limit);
    void setInviteOnly(bool mode);
    void setTopicRestricted(bool mode);


    
    void addClient(Client& cli);
    void removeClient(Client& cli, int indxcmd);
    void addOperator(Client& Ope);
    void removeOperator(Client& Ope);

    void addInvitee(Client& cli);
    bool isInvitee(Client& cli);
    bool hasPermission(Client &cli);
    void refrechChannel(Client &cli);
    void broadcastMessage(std::string msg);
};

#endif




