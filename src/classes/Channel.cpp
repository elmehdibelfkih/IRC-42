/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:09 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/15 04:03:18 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

Channel::Channel()
{
    this->_userLimit = 1;
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

void Channel::setChannelName(std::string newName, Client setter)
{
    (void)newName;
    (void)setter;
}

void Channel::setpassWord(std::string newpassWord, Client setter)
{
    (void)newpassWord;
    (void)setter;
    
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
    (void)cli;
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
