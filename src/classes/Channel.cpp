/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:09 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/07/11 07:24:47 by ybouchra         ###   ########.fr       */
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


void Channel::setMode(std::string newMode)
{
    this->_mode = newMode;

    
}



void Channel::addOperators(Client ope)
{
    (void)ope;
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
std::string Channel::getTime() const
{
    std::time_t currentTime = std::time(0);
    std::tm* localTime = std::localtime(&currentTime);
    char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return(std::string(buffer));
}
void Channel::addClient(Client cli)
{
    this->_clients.insert(std::pair<std::string ,Client>(cli.getNickName(), cli));

        cli.sendMsg(RPL_TOPIC(cli.getNickName(), this->getChannelName(),this->getTopic()));
        cli.sendMsg(RPL_TOPICWHOTIME(cli.getNickName(), this->getChannelName(),this->_setterCl.nickName, this->_setterCl.time));
        cli.sendMsg(RPL_NAMREPLY(cli.getNickName(), this->getMode(), this->getChannelName(), cli.getNickName()));
        cli.sendMsg(RPL_ENDOFNAMES(cli.getNickName(), this->getChannelName()));
    if(this->getTopic().empty())
        cli.sendMsg(RPL_NOTOPIC(cli.getNickName(), this->getChannelName()));
    
        
    
}
void Channel::removeClient(Client cli)
{
    // std::map<std::string, Client> ::iterator it = this->_clients.begin();
    // std::map<std::string, Client> ::iterator it1 = this->_clients.begin();
    // for(; it != this->_clients.end(); it++)
    //         std::cout << "---->  :" << it->first << "\n";
    this->_clients.erase(cli.getNickName());
    
    // for(; it1 != this->_clients.end(); it1++)
    //         std::cout << "++++>  :" << it1->first << "\n";
}

void Channel::setTopic(std::string newTopic, Client setter)
{
    this->_topic = newTopic;
    this->_setterCl.nickName = setter.getNickName();
    this->_setterCl.time = this->getTime();

    std::map<std::string, Client> ::iterator it = this->_clients.begin();
    for(; it != this->_clients.end(); it++)
        {
            it->second.sendMsg(RPL_TOPIC(it->second.getNickName(),this->getChannelName(), this->getTopic()));
            it->second.sendMsg(RPL_TOPICWHOTIME(it->first, this->getChannelName(), this->_setterCl.nickName, this->_setterCl.time));
        }
    
}

bool Channel::hasPermission(Client cli)
{
    if (this->getMode() == "+t")
    {
        if (cli.isOperator() || cli.isHalfOperator())
            return true; 
        return false; 
    }
        return true;
}


void Channel::brodcastMessage(Client sender, std::string msg)
{
    (void)sender;

    std::map<std::string, Client> ::iterator it = this->_clients.begin();
        for(; it != this->_clients.end(); it++)
            {
                if(sender.getNickName() == it->second.getNickName())
                    continue;
                it->second.sendMsg( sender.getNickName() + ": " + msg + " " + this->getTime());
            }
            
}