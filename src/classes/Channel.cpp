/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:09 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/23 12:58:41 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

Channel::Channel()
{
}
Channel::Channel(std::string channelname, std::string key) :_channelName(channelname), _passWord(key)
{
    this->_topic                = "";
    this->_userLimit            = -1;    //default value ofthe number of users who can join the channel.
    this->_mode.inviteOnly      = false; // any one can join to the channel ;
    this->_mode.topicRestricted = false; // any one can set the topic of the channel ;
    this->_mode.userLimit       = false; // No limit on the number of users who can join the channel.
    this->_mode.requiredKey     = false; // the key of channel required .
    this->_setterCl.nickName    = "";
    this->_setterCl.time        = this->getTime();
}

Channel& Channel::operator=(const Channel& obj)
{
    if (this != &obj)
    {
        this->_channelName = obj._channelName;
        this->_userLimit = obj._userLimit;
        this->_passWord = obj._passWord;
        this->_topic = obj._topic;
        this->_mode.inviteOnly = obj._mode.inviteOnly;
        this->_mode.topicRestricted = obj._mode.topicRestricted;
        this->_mode.userLimit = obj._mode.userLimit;
        this->_mode.requiredKey = obj._mode.requiredKey;
        this->_clients = obj._clients;
        this->_invitees = obj._invitees;
        this->_setterCl.nickName = obj._setterCl.nickName;
        this->_setterCl.time = obj._setterCl.time;
    }
    return *this;
}

Channel::Channel(const Channel& obj)
{
    *this = obj;
}

Channel::~Channel()
{
    this->_clients.clear();
    this->_invitees.clear();
}


std::string Channel::getChannelName() const
{
    return this->_channelName;
}

std::string Channel::getpassWord() const
{
    return this->_passWord;
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

bool Channel::getMode(char token) const
{
    if(token == 'i')
       return this->_mode.inviteOnly;
    if(token == 't')
        return this->_mode.topicRestricted;
    if(token == 'l')
        return this->_mode.userLimit;
    if(token == 'k')
        return this->_mode.requiredKey;
    return false;
}

 int Channel::getUserlimit() const
 {
    return(this->_userLimit);
 }

void Channel::setChannelName(std::string newName)
{
    this->_channelName = newName;
}

void Channel::setpassWord(std::string newpassWord)
{
    this->_passWord = newpassWord;
    
}

std::string Channel::getTime() const {
    std::time_t currentTime = std::time(0);
    std::tm localTime = *std::localtime(&currentTime); 
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return std::string(buffer);
}


std::string Channel::showModes() const
{
    std::string modes = "";
    
    if(this->getMode('i'))
        modes+= 'i';
    if(this->getMode('t'))
        modes+= 't';
    if(this->getMode('l'))
        modes+= 'l';
    if(this->getMode('k'))
        modes+= 'k';
    return(modes);
}

void Channel::addClient(Client& cli)
{

    cli.setnbrChannels('+');
    this->_clients.insert(std::pair<std::string ,Client>(cli.getNickName(), cli));
    if(this->_clients.size() == 1)
        this->_clients[cli.getNickName()].setOperStatus(true);

        
    this->broadcastMessage(cli , RPL_JOIN(cli.getNickName(), cli.getUserName(), cli.getIP(), this->getChannelName() ), true);
    // if(this->getTopic().size() == 0)
    //     cli.sendMsg(RPL_NOTOPIC(cli.getNickName(), this->getChannelName()));
    // else
    // {
    //     cli.sendMsg(RPL_TOPIC(cli.getNickName(), this->getChannelName(), this->getTopic()));
    //     cli.sendMsg(RPL_TOPICWHOTIME(cli.getNickName(), this->getChannelName(),this->_setterCl.nickName, this->_setterCl.time));
    // }
    
    // Send RPL_NAMREPLY refrech list users in the channel
    std::string userList; 
    for (std::map<std::string, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        if(this->hasPermission(it->second)  )// Check if the client is an operator
            userList += "@" + it->second.getNickName() + " ";
        else
            userList += it->second.getNickName() + " ";
    }
        
        cli.sendMsg(RPL_NAMREPLY(cli.getNickName(), "=",this->getChannelName(), trimFunc(userList) ));
        cli.sendMsg(RPL_ENDOFNAMES(cli.getNickName(), this->getChannelName()));
    }




void Channel::removeClient(Client& cli)
{
    cli.setnbrChannels('-');
    
    
    this->broadcastMessage(cli, SERVERNAME + " " + cli.getNickName() + " has left " + this->getChannelName() + "\r\n", true);
    this->_clients.erase(cli.getNickName());

    std::string userList; 
    for (std::map<std::string, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        if (this->hasPermission(it->second)) // Check if the client is an operator
            userList += "@" + it->second.getNickName() + " ";
        else
            userList += it->second.getNickName() + " ";
    }

   for (std::map<std::string, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        it->second.sendMsg(RPL_NAMREPLY(it->second.getNickName(), "=", this->getChannelName(), trimFunc(userList)));
        it->second.sendMsg(RPL_ENDOFNAMES(it->second.getNickName(), this->getChannelName()));
    }
    
}

void Channel::setTopic(std::string newTopic, Client setter)
{
     if (this->_topic == newTopic)
        return;
    this->_topic = newTopic;
    this->_setterCl.nickName = setter.getNickName();
    this->_setterCl.time = this->getTime();

    this->broadcastMessage(setter, ":" + setter.getNickName() + "!" + setter.getUserName() + "@" + setter.getIP() +" TOPIC " + this->getChannelName() + " :" + this->getTopic() + "\r\n", true); 
   if (this->_topic.empty()) 
        this->broadcastMessage(setter, RPL_NOTOPIC(setter.getNickName(), this->getChannelName()), true);
 else {
        this->broadcastMessage(setter, RPL_TOPIC(setter.getNickName(), this->getChannelName(), this->getTopic()), true);
        this->broadcastMessage(setter, RPL_TOPICWHOTIME(setter.getNickName(), this->getChannelName(), this->_setterCl.nickName, this->_setterCl.time), true);
    }
}

bool Channel::hasPermission(Client cli)
{
    if(this->_clients.empty())
        return false;
    if(this->_clients[cli.getNickName()].getOperStatus())
        return true;
    return false;
        
}


void Channel::broadcastMessage( Client &sender, std::string msg, bool all)
{
    std::map<std::string, Client> ::iterator it = this->_clients.begin();
        for(; it != this->_clients.end(); it++)
            {
                if(!all && sender.getNickName() == it->second.getNickName())
                    continue;
                it->second.sendMsg( msg );
            }         
}


void Channel::setInviteOnly(bool mode)
    {
        this->_mode.inviteOnly = mode;
    }

void Channel::setTopicRestricted(bool mode)
    {
        this->_mode.topicRestricted = mode;
    }
void  Channel::setUserLimit(int limit)
    {
        this->_userLimit = limit ;
        this->_mode.userLimit = true;
    }