/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:09 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/05 00:22:23 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

Channel::Channel()
{
}
Channel::Channel(std::string channelname, std::string key) : _channelName(channelname), _passWord(key)
{
    this->_topic                = "";
    this->_userLimit            = -1;               // default value ofthe number of users who can join the channel.
    this->_mode.inviteOnly      = false;      // invite only: no one can join a channel whithout pre invite from operator the channel
    this->_mode.topicRestricted = false; // any one can set the topic of the channel .
    this->_mode.userLimit       = false;       // used to limit the number of users in the channel
    this->_mode.requiredKey     = false;     // password is required to join channel.
    this->_creationTime         = this->getTime();
}

Channel &Channel::operator=(const Channel &obj)
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
        this->_creationTime = obj._creationTime;

    }
    return *this;
}

Channel::Channel(const Channel &obj)
{
    *this = obj;
}

Channel::~Channel()
{
    this->_clients.clear();
    this->_invitee.clear();
    this->_operators.clear();
}

std::string Channel::getChannelName() 
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
    if (token == 'i')
        return this->_mode.inviteOnly;
    if (token == 't')
        return this->_mode.topicRestricted;
    if (token == 'l')
        return this->_mode.userLimit;
    if (token == 'k')
        return this->_mode.requiredKey;
    return false;
}

int Channel::getUserlimit() const
{
    return (this->_userLimit);
}

void Channel::setChannelName(std::string newName)
{
    this->_channelName = newName;
}

void Channel::setpassWord(std::string newpassWord)
{
    this->_passWord = newpassWord;

}

std::string Channel::getTime() const
{

    std::time_t currentTime = std::time(0);
    std::ostringstream oss;
    oss << currentTime;  // Convert time_t to string
    return oss.str();    // Return the string representation

}

std::string Channel::getCreationTime() const
{
    return this->_creationTime;
}

std::string Channel::showModes() const
{
    std::string modes = "";

    if (this->getMode('t'))
        modes += 't';
    if (this->getMode('i'))
        modes += 'i';
    if (this->getMode('l'))
        modes += 'l';
    if (this->getMode('k'))
    modes += 'k';
        
   if (!modes.empty())
        modes.insert(modes.begin(), '+');
    return (modes);
}
void Channel::refrechChannel(Client &cli)
{
    std::string userList;
    for (std::map<std::string, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        if (this->hasPermission(*it->second))
            userList += "@" + (*it->second).getNickName() + " ";
        else
            userList += (*it->second).getNickName() + " ";
    }
    
    cli.sendMsg(RPL_NAMREPLY(cli.getNickName(), '=', this->getChannelName(), trimFunc(userList)));
    cli.sendMsg(RPL_ENDOFNAMES(cli.getNickName(), this->getChannelName()));
}

std::string    reply_join(Client clt, Channel ch)
{
    std::stringstream ss;
    ss << ":" + clt.getNickName() + "!~" + clt.getUserName() + "@" + clt.getIP() + " JOIN " + ch.getChannelName() + "\r\n";
    ss << ":ircserver " << "MODE " + ch.getChannelName() << " " << ch.showModes() << "\r\n";
    return ss.str();
}

void Channel::addClient(Client &cli)
{
    cli.setnbrChannels('+');
    this->_clients.insert(std::pair<std::string, Client *>(cli.getNickName(), &cli));
    if (this->_clients.size() == 1)
        this->addOperator(cli);
        
    this->broadcastMessage(reply_join(cli, *this));
    if(!this->getTopic().empty())
    {
        cli.sendMsg(RPL_TOPIC(cli.getNickName(), this->getChannelName(), this->getTopic()));
        cli.sendMsg(RPL_TOPICWHOTIME(cli.getNickName(), this->getChannelName(),this->_setterCl.nickName, this->_setterCl.time));
    }
    refrechChannel(cli);
    
}

void Channel::removeClient(Client &cli, int indexcmd)
{
    cli.setnbrChannels('-');

    if (indexcmd == KICK)
        this->broadcastMessage(":" + cli.getNickName() + " has been kicked from " + this->getChannelName() + "\r\n");
    else if (indexcmd == PART)
        this->broadcastMessage(":" + cli.getNickName() + " has left " + this->getChannelName() + "\r\n");

    if (this->hasPermission(cli))
        removeOperator(cli);

    this->_clients.erase(cli.getNickName());
    refrechChannel(cli);

}


void Channel::addOperator(Client &Ope)
{
   if(this->hasPermission(Ope))
        return;
    this->_operators.push_back(Ope.getNickName());
    this->refrechChannel(Ope);

}

void Channel::removeOperator(Client &Ope)
{
        
    std::vector<std::string> ::iterator it = std::find(this->_operators.begin(), this->_operators.end(), Ope.getNickName());
        if(it != this->_operators.end())
            {
                this->_operators.erase(it);
                this->refrechChannel(Ope);
            }
}


void Channel::setTopic(std::string newTopic, Client setter)
{

    this->_topic             = newTopic;
    this->_setterCl.nickName = setter.getNickName();
    this->_setterCl.userName = setter.getUserName();
    this->_setterCl.ip       = setter.getIP();
    this->_setterCl.time     = this->getTime();


    this->broadcastMessage(":" + setter.getNickName() + "!" + setter.getUserName() + "@" + setter.getIP() + " TOPIC " + this->getChannelName() + " :" + this->getTopic() + "\r\n");

        setter.sendMsg(RPL_TOPIC(setter.getNickName(), this->getChannelName(), this->getTopic()));
        std::string setterInfo = _setterCl.nickName + "!~" + _setterCl.userName + "@" +_setterCl.ip;
        setter.sendMsg(RPL_TOPICWHOTIME(setter.getNickName(), this->getChannelName(), setterInfo, this->_setterCl.time));

}

bool Channel::hasPermission(Client &cli)
{
return std::find(this->_operators.begin(), this->_operators.end(), cli.getNickName()) != this->_operators.end();
}

void Channel::broadcastMessage(std::string msg)
{
    std::map<std::string, Client *>::iterator it = this->_clients.begin();

    for (; it != this->_clients.end(); it++)
    {
        it->second->sendMsg(msg);
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
void Channel::setUserLimit(int limit)
{
    this->_userLimit = limit;
}

void Channel::addInvitee(Client &cli)
{
    this->_invitee.push_back(cli.getNickName());
}


bool Channel::isInvitee(Client &cli)
{
    if (this->_invitee.empty())
        return false;
    for (std::vector<std::string>::iterator it = this->_invitee.begin(); it != this->_invitee.end(); ++it)
    {
        if (*it == cli.getNickName())
            return true;
    }
    return false;
}
