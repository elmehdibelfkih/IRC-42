/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:16:50 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/08 16:10:55 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include "Client.hpp"
#include "replies.hpp"
#include "IRC.hpp"

class Client;

typedef struct s_SetterCl // For Topic 
{
    std::string nickName;
    std::string time;
}t_SetterCl;

typedef struct s_Mode
{
    bool i; // invite only
    bool t; // topic restricted
    bool k; // key required
    bool l; // user limit
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
    std::vector<std::string>    	_operators;

public:
    Channel();
    Channel(std::string channelName, std::string key, t_Mode mode);
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
    Client*     getClientByNickName(std::string nick);
    bool isOperator(Client &cli);
    
    
    
    // setters
    void setChannelName(std::string newName);
    void setpassWord(std::string newpassWord);
    void setTopic(std::string newTopic, Client setter);
    void setUserLimit(int limit);
    void setInviteOnly(bool mode);
    void setTopicRestricted(bool mode);
    void setOperMode(Client &cli, bool mode);

    void setModes(char mode, bool value);

    
    void addClient(Client *cli);
    void removeClient(Client& cli, int indxcmd);
    void addInvitee(Client& cli);
    bool isInvitee(Client& cli);
    void refrechChannel(Client cli);
    void broadcastMessage(std::string msg);
    void broadcastMessage(std::string msg, Client &cli);
    std::string listusers( void );
};

std::vector < std::pair<std::string, std::string> > parseModeArgs(std::vector<std::string> modeArgse, bool &syntaxerror);

void  inviteOnlyMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode); // mode i
void  limitMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode); // mode l
void  keyMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode); // mode k
void  operatorMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode); // mode o
void  topicMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode); // mode t   

#endif




