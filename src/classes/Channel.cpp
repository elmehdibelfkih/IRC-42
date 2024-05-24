/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:09 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/05/24 03:15:44 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

Channel::Channel()
{
    this->_userLimit = 100;
    this->_channelName = "";
    this->_passWord = "";
    this->_topic = "";
    this->_mode = "";
}

Channel& Channel::operator=(const Channel& obj)
{
    if (this != &obj)
    {
        this->_userLimit = obj._userLimit;
        this->_channelName = obj._channelName;
        this->_passWord = obj._passWord;
        this->_topic = obj._topic;
        this->_mode = obj._mode;
        this->_clients = obj._clients;
        this->_operators = obj._operators;
        this->_invitees = obj._invitees;
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
    this->_operators.clear();
    this->_invitees.clear();
}

/////////////////////////////////////////////////////////////////////////////////

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

std::string Channel::getMode() const
{
    return this->_mode;
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

void Channel::setTopic(std::string newTopic, Client setter)
{
    (void)newTopic;
    (void)setter;
    
}

void Channel::setMode(std::string newMode, Client setter)
{
    (void)newMode;
    (void)setter;
    
}

void Channel::addClient(Client cli)
{
    this->_clients.insert(std::pair<std::string ,Client>(cli.getNickName(), cli));

        cli.sendMsg(RPL_TOPIC(cli.getNickName(), this->getChannelName(),this->getTopic()));
        cli.sendMsg(RPL_TOPICWHOTIME(cli.getNickName(), this->getChannelName(),(std::string)"@@IP", (std::string)"time"));
        cli.sendMsg(RPL_NAMREPLY(cli.getNickName(), this->getMode(), this->getChannelName(), cli.getNickName()));
        cli.sendMsg(RPL_ENDOFNAMES(cli.getNickName(), this->getChannelName()));
        
    
}

void Channel::addOperators(Client ope)
{
    (void)ope;
}

void Channel::addInvited(Client inv)
{
    (void)inv;
}

void Channel::brodcastMessage(std::string message, Client sender)
{
    (void)message;
    (void)sender;
}

bool Channel::joinChannel(Client cli)
{
    (void)cli;
    return false;
}

void Channel::removeClient(int fd)
{
    (void)fd;
}
bool Channel::isBannedFromChannel(Channel ch, Client cl)
{

    std::string mask =  cl.getNickName() + "!" + cl.getUserName() + "@" + cl.getIP();
    
    if(ch._bannedClient.empty())
        return false;

        std::map<std::string, Client>::iterator it = this->_bannedClient.lower_bound(mask);
        if(it != this->_bannedClient.end() && it->first == mask)
            return(true);
        return(false);

}

