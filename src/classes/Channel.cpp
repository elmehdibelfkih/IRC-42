/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:09 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/08 16:11:47 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(std::string channelName, std::string key, t_Mode mode) 
    : _channelName(channelName), _passWord(key), _mode(mode)
{
    this->_userLimit = -1;
}

Channel &Channel::operator=(const Channel &obj)
{
    if (this != &obj)
    {
        this->_channelName = obj._channelName;
        this->_passWord = obj._passWord;
        this->_topic = obj._topic;
        this->_userLimit = obj._userLimit;
        this->_mode = obj._mode;
        this->_invitee = obj._invitee;
        this->_clients = obj._clients;
        this->_operators = obj._operators;
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

Client* Channel::getClientByNickName(std::string nick)
{
    for (std::map<std::string, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
    {
        if (it->first == nick)
            return it->second;
    }
    return NULL;
}


bool Channel::getMode(char token) const
{
    if (token == 'i')
        return this->_mode.i;
    if (token == 't')
        return this->_mode.t;
    if (token == 'l')
        return this->_mode.l;
    if (token == 'k')
        return this->_mode.k;
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
    this->_mode.k = true;
    this->_passWord = newpassWord;
}

std::string Channel::showModes() const
{
    std::string modes = "";

    if (this->getMode('k'))
        modes += 'k';
    if (this->getMode('i'))
        modes += 'i';
    if (this->getMode('t'))
        modes += 't';
    if (this->getMode('l'))
        modes += 'l';
    return (modes);
}

void Channel::setModes(char mode, bool value)
{
    if (mode == 'k')
        this->_mode.k = value;
    if (mode == 'i')
        this->_mode.i = value;
    if (mode == 't')
        this->_mode.t = value;
    if (mode == 'l')
        this->_mode.l = value;
}


void Channel::refrechChannel(Client cli)
{
    std::string userList;
    for (std::map<std::string, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        if (this->isOperator(*it->second))
            userList += "@" + (*it->second).getNickName() + " ";
        else
            userList += (*it->second).getNickName() + " ";
    }

    cli.sendMsg(RPL_NAMREPLY(cli.getNickName(), "=", this->getChannelName(), trimFunc(userList)));
    cli.sendMsg(RPL_ENDOFNAMES(cli.getNickName(), this->getChannelName()));
}

std::string Channel::listusers( void )
{
    std::string userList;
    for (std::map<std::string, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        if (this->isOperator(*it->second))
            userList += "@" + (*it->second).getNickName() + " ";
        else
            userList += (*it->second).getNickName() + " ";
    }
    return userList;
}

std::string    reply_join(Client clt, Channel ch) {
    std::stringstream ss;
    
    ss << ":" + clt.getNickName() + "!~" + clt.getUserName() + "@" + clt.getIP() + " JOIN " + ch.getChannelName() + "\r\n";
    ss << ":ircserver " << "MODE " + ch.getChannelName() << " +" << ch.showModes() << "\r\n";
    ss << ":ircserver " << "353 " << clt.getNickName() << " @ " << ch.getChannelName() << " :" << ch.listusers() << "\r\n";
    ss << ":ircserver 366 " << clt.getNickName() << " " << ch.getChannelName() << " :End of /NAMES list.\r\n";
    return ss.str();
}

void Channel::addClient(Client *cli)
{
    cli->setnbrChannels('+');
    this->_clients.insert(std::pair<std::string, Client *>(cli->getNickName(), cli));
    if (this->_clients.size() == 1)
        this->_operators.push_back(cli->getNickName());
    this->broadcastMessage(reply_join(*cli, *this));
    if(!this->getTopic().empty())
    {
        cli->sendMsg(RPL_TOPIC(cli->getNickName(), this->getChannelName(), this->getTopic()));
        cli->sendMsg(RPL_TOPICWHOTIME(cli->getNickName(), this->getChannelName(),this->_setterCl.nickName, this->_setterCl.time));
    }
    // else   
    //     cli->sendMsg(RPL_NOTOPIC(cli->getNickName(), this->getChannelName()));
    refrechChannel(*cli);
}

void Channel::removeClient(Client &cli, int indexcmd)
{
    cli.setnbrChannels('-');

    if (indexcmd == KICK)
        this->broadcastMessage(":" + cli.getNickName() + " has been kicked from " + this->getChannelName() + "\r\n");
    else if (indexcmd == PART)
        this->broadcastMessage(":" + cli.getNickName() + " has left " + this->getChannelName() + "\r\n");

    refrechChannel(cli);
    this->_clients.erase(cli.getNickName());

}

void Channel::setTopic(std::string newTopic, Client setter)
{
    if (this->_topic == newTopic || newTopic.empty())
        return;
    this->_topic = newTopic;
    this->_setterCl.nickName = setter.getNickName();
    this->_setterCl.time = getCurrentTime();

    this->broadcastMessage(":" + setter.getNickName() + "!" + setter.getUserName() + "@" + setter.getIP() + " TOPIC " + this->getChannelName() + " :" + this->getTopic() + "\r\n");
    if (this->_topic.empty())
        this->broadcastMessage(RPL_NOTOPIC(setter.getNickName(), this->getChannelName()));
    else
    {
        this->broadcastMessage(RPL_TOPIC(setter.getNickName(), this->getChannelName(), this->getTopic()));
        this->broadcastMessage(RPL_TOPICWHOTIME(setter.getNickName(), this->getChannelName(), this->_setterCl.nickName, this->_setterCl.time));
    }
}

void Channel::broadcastMessage(std::string msg)
{
    for (std::map<std::string, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
        it->second->sendMsg(msg);
}


void Channel::broadcastMessage(std::string msg, Client &cli)
{
    for (std::map<std::string, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
    {
        if (it->second->getNickName() != cli.getNickName())
            it->second->sendMsg(msg);
    }
}

bool Channel::isOperator(Client &cli)
{
    for (std::vector<std::string>::iterator it = this->_operators.begin(); it != this->_operators.end(); ++it)
    {
        if (*it == cli.getNickName())
            return true;
    }
    return false;
}

void Channel::setOperMode(Client &cli, bool mode)
{
    if (mode && !this->isOperator(cli))
        this->_operators.push_back(cli.getNickName());
    else if (this->isOperator(cli) && !mode)
    {
        for (std::vector<std::string>::iterator it = this->_operators.begin(); it != this->_operators.end(); it++)
        {
            if (*it == cli.getNickName())
            {
                this->_operators.erase(it);
                break;
            }
        }
    }
}


void Channel::setInviteOnly(bool mode)
{
    this->_mode.i = mode;
}

void Channel::setTopicRestricted(bool mode)
{
    this->_mode.t = mode;
}
void Channel::setUserLimit(int limit)
{
    this->_userLimit = limit;
    this->_mode.l = true;
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
