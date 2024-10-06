/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 19:06:26 by ybouchra          #+#    #+#             */
/*   Updated: 2024/10/06 06:40:57 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Server.hpp"
#include "../../inc/IRC.hpp"


void Server::passCommand(int i)
{

    std::string key = this->_clients[i].getMessage().getToken();
    key = trimFunc(key);
    
    std::string nickname = this->_clients[i].getNickName();
    if (nickname.empty())
        nickname = "*";
        
    if (key.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(nickname, "PASS"));

    if (this->_clients[i].getAuthenticate())
        return this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName()));

    if (key != this->_passWord)
        return this->_clients[i].sendMsg(ERR_PASSWDMISMATCH(nickname));

    this->_clients[i].setPass(true);
    this->_clients[i].sendMsg(RPL_VALIDPASS());
}

void Server::nickCommand(int i)
{
    std::string params = this->_clients[i].getMessage().getToken();
    params = trimFunc(params);
    std::string nickname = this->_clients[i].getNickName();
    if (nickname.empty())
        nickname = "*";

    if (params.empty())
        return this->_clients[i].sendMsg(ERR_NONICKNAMEGIVEN(nickname));
    if (this->_clients[i].getPass() == false)
        return this->_clients[i].sendMsg(ERR_NOTREGISTERED(nickname));
    if (!this->checkNickName(params))
        return this->_clients[i].sendMsg(ERR_ERRONEUSNICKNAME(nickname, this->_clients[i].getMessage().getToken()));
    if (this->getClientByNickName(params) != NULL)
        return this->_clients[i].sendMsg(ERR_NICKNAMEINUSE(nickname, this->_clients[i].getMessage().getToken()));
    else
    {
        std::string msg = CHANGENICK(nickname , _clients[i].getUserName(), _clients[i].getIP(), params);
        std::map<std::string, Channel> ::iterator it = this->_channels.begin();
        std::string oldnickname = this->_clients[i].getNickName();
        this->_clients[i].setNickName(params);
        for(; it != _channels.end(); ++it)
        {
            if (it->second._clients.find(oldnickname) != it->second._clients.end()) {
                it->second._clients.erase(oldnickname);
                it->second._clients.insert(std::make_pair(params, &this->_clients[i]));
                
                it->second.broadcastMessage(this->_clients[i], msg, true);
            }
        }
        this->_clients[i].setNickName(params);
        return this->_clients[i].sendMsg(RPL_VALIDNICK());
    }
}

void Server::userCommand(int i)
{
    std::string params;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "USER")));
        
    if (this->_clients[i].getAuthenticate()) // Check if the user is already registered
        return (this->_clients[i].sendMsg(ERR_ALREADYREGISTERED(this->_clients[i].getNickName())));
    std::vector<std::string> argsVec = splitString(params, ' ');
    
    if (argsVec.empty() || argsVec.size() != 4)
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "USER")));
    
    if (!checkUserName(argsVec[0])) // check param if valide.
        return this->_clients[i].sendMsg(ERR_ERRONEUSUSERNAME(this->_clients[0].getNickName(), params));
    
    if (this->_clients[i].getNickName().empty() || this->_clients[i].getPass() == false)
        return this->_clients[i].sendMsg(ERR_NOTREGISTERED(this->_clients[i].getNickName()));
        
    this->_clients[i].setUserName(argsVec[0]);
    this->_clients[i].setAuthenticate(true);

    this->_clients[i].sendMsg(RPL_WELCOME(this->_clients[i].getNickName(), this->_clients[i].getUserName(), this->_clients[i].getIP()));
    this->_clients[i].sendMsg(RPL_YOURHOST(this->_clients[i].getNickName()));
    this->_clients[i].sendMsg(RPL_CREATED(this->_clients[i].getNickName(), this->_clients[i].getTime()));
    
}


void Server::partCommand(int i)
{
    std::string params, channelname, reason;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PART"));
    

    std::vector<std::string> argsVec = splitString(params, ' ');
        if(argsVec.empty())
            return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PART"));
        
    if(argsVec.size() > 1)
        reason = params.substr(argsVec[0].size() + 1); 
    std::istringstream iss(argsVec[0]);
    
    while(std::getline(iss, channelname, ','))
    {
        if (!findChannelName(channelname))
            {
                this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));
                continue;
            } 
        if (!is_memberInChannel(channelname, this->_clients[i]))
            {
                this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelname));
                continue;
            }
        if (!reason.empty() && reason[0] != ':')
        reason = ":" + reason; 

        std::string partMessage = ":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PART " + channelname + " " + reason + "\r\n";
    this->_channels[channelname].broadcastMessage(this->_clients[i], partMessage, true);
    this->_channels[channelname].removeClient(this->_clients[i], PART);
    if(this->_channels[channelname]._clients.empty())
        this->_channels.erase(channelname);
        
    }
}


void Server::kickCommand(int i)
{
    std::string params, reason;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "KICK"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.size() < 2)
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "KICK"));

    std::string &channelname = argsVec[0];
    std::string &kickedusers = argsVec[1];

    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));

    if (!this->_channels[channelname].hasPermission(_clients[i]))
        return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));

    if (argsVec.size() > 2)  // reason is provided
        reason = params.substr(channelname.size() + kickedusers.size() + 2); 
    else
        reason = "No reason provided";

    if (!reason.empty() && reason[0] != ':')
        reason = ":" + reason;

    std::istringstream iss(kickedusers);
    std::string k_user;
    while (std::getline(iss, k_user, ',')) {
        Client *cl = getClientByNickName(k_user); 
        if (cl == NULL) {
            this->_clients[i].sendMsg(ERR_NOSUCHNICK(k_user));
            continue;
        }

        if (!is_memberInChannel(channelname, *cl))
            return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(k_user, channelname));

        this->_channels[channelname].broadcastMessage(this->_clients[i], RPL_KICK(_clients[i].getNickName(), _clients[i].getUserName(), _clients[i].getIP(), channelname, k_user, reason), true);
        this->_channels[channelname].removeClient(*cl, KICK);
    }
}


void Server::topicCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "TOPIC"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "TOPIC"));
    std::string channelname = argsVec[0];
    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));

    if (!is_memberInChannel(channelname, this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelname));

    if (this->_channels[channelname].getMode('t') == true)
    {
        if (!this->_channels[channelname].hasPermission(_clients[i]))
            return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));
    }

    if (argsVec.size() == 1)// display current topic
    {
        if (this->_channels[channelname].getTopic().empty())
            return this->_clients[i].sendMsg(RPL_NOTOPIC(this->_clients[i].getNickName(), channelname));
        else
        {
            this->_clients[i].sendMsg(RPL_TOPIC(this->_clients[i].getNickName(), channelname, this->_channels[channelname].getTopic()));
            std::string setterInfo = _channels[channelname]._setterCl.nickName + "!~" + _channels[channelname]._setterCl.userName + "@" + _channels[channelname]._setterCl.ip;
            this->_clients[i].sendMsg(RPL_TOPICWHOTIME(_clients[i].getNickName() ,channelname, setterInfo, _channels[channelname]._setterCl.time ));
            return;
        }
    }
    if (argsVec.size() > 1)    // topic is provided
    {
        std::string newTopic = params.substr(argsVec[0].size() + 1); // +1 to skip the space after the channel name
        if (!newTopic.empty() && newTopic[0] == ':')
            newTopic = newTopic.substr(1); // rm ':' from topic

        this->_channels[argsVec[0]].setTopic(newTopic, this->_clients[i]);
    }
}

void Server::privmsgCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PRIVMSG"));
    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.size() < 2)
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PRIVMSG"));

    std::string target = argsVec[0];
    if (target.at(0) == '#') // target channel.
    {
        if (findChannelName(target) == false)
            return this->_clients[i].sendMsg(ERR_NOSUCHSERVER(this->_clients[i].getNickName()));
        if (!is_memberInChannel(target, this->_clients[i]))
            return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), target));
        
        std::string msg = params.substr(target.size() + 1) ;
        this->_channels[target].broadcastMessage(this->_clients[i], ":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PRIVMSG " + target + " " + msg + "\r\n", false);
    }
    else // target client.
    {

        Client &sender = this->_clients[i];       // sender .
        Client *cl = getClientByNickName(target); // reciever client .
        if (cl == NULL)
            return this->_clients[i].sendMsg(ERR_NOSUCHNICK(target));
        cl->sendMsg(":" + sender.getNickName() + "!~" + sender.getUserName() + "@" + sender.getIP() + " PRIVMSG " + target + "" + params.substr(argsVec[0].size()) + "\r\n");
    }
}

void Server::listCommand(int i)
{
    std::string params = this->_clients[i].getMessage().getToken();
    std::vector<std::string> argsVec = splitString(params, ' ');

    if (argsVec.empty())
    {
        for (std::map<std::string, Channel>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
        {
            const std::string &channelName = it->first;
            const Channel &channel = it->second;

            // Send = channel name : nbr of users in the channel : topic
            this->_clients[i].sendMsg(RPL_LIST(this->_clients[i].getNickName(), channelName, channel._clients.size(), channel.getTopic()));
        }
    }
    else
    {
        if (!findChannelName(argsVec[0]))
            return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));
        Channel &channel = this->_channels[argsVec[0]];
        this->_clients[i].sendMsg(RPL_LIST(this->_clients[i].getNickName(), argsVec[0], channel._clients.size(), channel.getTopic()));
    }
    this->_clients[i].sendMsg(RPL_LISTEND(this->_clients[i].getNickName()));
}


void Server::inviteCommand(int i)
{
    std::string params;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "INVITE"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.size() < 2)
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "INVITE"));

    std::string &inviteduser = argsVec[0]; 
    std::string &channelname = argsVec[1]; 

    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));

    if (!is_memberInChannel(channelname, this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelname));

    if (this->_channels[channelname].getMode('i') && !this->_channels[channelname].hasPermission(_clients[i]))
        return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));

    Client *cl = getClientByNickName(inviteduser);
    if (cl == NULL)
        return this->_clients[i].sendMsg(ERR_NOSUCHNICK(inviteduser));

    if (is_memberInChannel(channelname, *cl))
        return this->_clients[i].sendMsg(ERR_USERONCHANNEL(inviteduser, channelname));

    this->_channels[channelname].addInvitee(*cl);

    // Notify the invited user about the invitation
    cl->sendMsg(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));

    // Send confirmation to the inviter
    this->_clients[i].sendMsg(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));


}


std::pair<std::vector<std::string>, std::vector<std::string> > parseChannels(std::string params, bool &syntaxerror)
{
    std::vector<std::string> channels, keys;

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty() || argsVec.size() > 2)
    {
        syntaxerror = true;
        return std::pair<std::vector<std::string>, std::vector<std::string> >();
    }
    channels = splitString(argsVec[0], ',');
    if (argsVec.size() > 1)
        keys = splitString(argsVec[1], ',');
        
    if (keys.size() < channels.size())
        keys.resize(channels.size(), "");

return std::pair< std::vector<std::string>, std::vector<std::string> > (channels, keys);

    return std::make_pair(channels, keys);
}

void Server::joinCommand(int i)
{
    bool syntaxerror = false;
    std::string params = this->_clients[i].getMessage().getToken();
    if (params.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "JOIN"));
    std::pair<std::vector<std::string>, std::vector<std::string> > channels = parseChannels(params, syntaxerror);

        if (syntaxerror)
            return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "JOIN"));
    for (size_t j = 0; j < channels.first.size(); j++) 
    {
        std::string channelname = channels.first[j];
        std::string key = channels.second[j];

        if (!isValidChannelName(channelname))
            return this->_clients[i].sendMsg(ERR_BADCHANMASK(this->_clients[i].getNickName(), channelname));
        if (this->_clients[i].getnbrChannels() >= LIMITCHANNELS)
            return this->_clients[i].sendMsg(ERR_TOOMANYCHANNELS(this->_clients[i].getNickName(), channelname));
        if (findChannelName(channelname))
        {
            Channel &channel = this->_channels[channelname]; 

            if (channel.getMode('k') && channel._passWord != key)
                return this->_clients[i].sendMsg(ERR_BADCHANNELKEY(this->_clients[i].getNickName(), channelname));
            if (is_memberInChannel(channelname, this->_clients[i]))
                return _clients[i].sendMsg(ERR_USERONCHANNEL(this->_clients[i].getNickName(), channelname));
            if (channel.getMode('l') && channel._clients.size() >= (size_t)channel.getUserlimit())
                return this->_clients[i].sendMsg(ERR_CHANNELISFULL(this->_clients[i].getNickName(), channelname));
            if (channel.getMode('i') && !channel.isInvitee(this->_clients[i]))
                return this->_clients[i].sendMsg(ERR_INVITEONLYCHAN(this->_clients[i].getNickName(), channelname));
        }
        else
        {
            if (key.empty())
                this->createChannel(channelname, key);
            else
                this->createChannel(channelname, key);
        }
        this->_channels[channelname].addClient(this->_clients[i]);
        if (this->_channels[channelname].isInvitee(this->_clients[i]))
        {
            std::vector<std::string>& inviteeList = this->_channels[channelname]._invitee;
            std::vector<std::string>::iterator it = std::find(inviteeList.begin(), inviteeList.end(), this->_clients[i].getNickName());

            if (it != inviteeList.end())
                inviteeList.erase(it);
        }
    }
}





void Server::applyModes(const std::string &modes, const std::vector<std::string> &argsVec, int clientIdx) {
    Client &client = this->_clients[clientIdx];
    std::string channelName = argsVec[0];
    Channel &channel = this->_channels[channelName];
    Client *targetClient = NULL;


    bool signal ;
    size_t index = 2;  
    
    for (size_t i = 0; i < modes.size(); ++i) {
        char modeChar = modes[i];

        if (modeChar == '+') {
            signal = true;
            continue;
        } else if (modeChar == '-') {
            signal = false;
            continue;
        }

        switch (modeChar) {
            case 'i': 
            if(channel.getMode('i') == signal)
                return;
                channel.setInviteOnly(signal);
                channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                    " MODE " + channelName + " " + (signal ? "+i" : "-i") + "\r\n", true);
                break;

            case 't': 
            if(channel.getMode('t') == signal)
                return;
                channel.setTopicRestricted(signal);
                channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                    " MODE " + channelName + " " + (signal ? "+t" : "-t") + "\r\n", true);
                break;

            case 'k':  
                    if (argsVec.size() <= index )
                        return;
                if (signal) {

                    std::string key = argsVec[index++];
                    channel._mode.requiredKey = true;
                    channel.setpassWord(key);
                    channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                        " MODE " + channelName + " +k " + key + "\r\n", true);
                } else
                {
                    channel._passWord.clear();
                    channel._mode.requiredKey = false;
                    channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                        " MODE " + channelName + " -k " + "*" + "\r\n", true);
                }
                break;

            case 'l':
                if (signal) {
                      if (argsVec.size() <=  index )
                        return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE"));

                    int limit = stringToInt(argsVec[index++]);
                    if (limit <= 0)
                        return;

                    channel.setUserLimit(limit);
                    channel._mode.userLimit = true;
                    channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                        " MODE " + channelName + " +l " + intToString(limit) + "\r\n", true);
                } 
                else{
                    if (channel.getMode('l') == signal)
                        return;
                    channel._mode.userLimit = false;
                    channel._userLimit = -1;
                    channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                        " MODE " + channelName + " -l " +"\r\n", true);
                }
                break;
                
            case 'o': 
                if (argsVec.size() <= index )
                    return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE"));

                if (!(targetClient = getClientByNickName(argsVec[index++])))
                    return client.sendMsg(ERR_NOSUCHNICK(argsVec[index - 1]));
      
                if (signal) {
                    if(channel.hasPermission(*targetClient))
                        return;
                  channel.addOperator(*targetClient);
                } 
                else {
                    if(!channel.hasPermission(*targetClient))
                        return;
                    channel.removeOperator(*targetClient);
                }
                
                channel.broadcastMessage(client, ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() +
                    " MODE " + channelName + " " + (signal ? "+o " : "-o ") + targetClient->getNickName() + "\r\n", true);
                break;
                
            case 'p':case 's':case 'n':case 'm':case 'r':case 'b':case 'v':case 'e':case 'q':case 'f':case 'c':case 'C':case 'z': break;
            
            default:
                return client.sendMsg(ERR_UNKNOWNMODE(client.getNickName(), "" + modeChar ));
                
        }
    }
}





void Server::modeCommand(int i)
{
    std::string params;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "MODE")));
    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty())
        return (this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "MODE")));

    std::string &channelName = argsVec[0];
    if (!channelName.empty() && channelName.at(0) == '#')
    {
        if (!this->_channels[channelName].hasPermission(_clients[i]))
            return (this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelName)));
        if (!findChannelName(channelName))
            return (this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!is_memberInChannel(channelName, this->_clients[i]))
            return (this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelName)));

        if (argsVec.size() == 1) // display modes
        {
            std::string key = (_channels[channelName].getMode('k') ? _channels[channelName].getpassWord() : "");
            std::string userlimit = "";
            if(_channels[channelName].getMode('l'))
                 userlimit = intToString(_channels[channelName].getUserlimit());
        
            this->_clients[i].sendMsg(RPL_CHANNELMODEIS(this->_clients[i].getNickName(), _channels[channelName].getChannelName(), _channels[channelName].showModes(), key, userlimit) );
            this->_clients[i].sendMsg(RPL_CREATIONTIME(this->_clients[i].getNickName(), _channels[channelName].getChannelName(), _channels[channelName].getCreationTime()));

            return ;
        }

        std::string &mode = argsVec[1];
        if (mode.empty())
            return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "MODE"));
   
        for (size_t j = 0; j < mode.size(); ++j)
        {
            if (std::string(VALIDMODES).find(mode[j]) == std::string::npos)
                return _clients[i].sendMsg(ERR_UNKNOWNMODE(_clients[i].getNickName(), "" + mode[j]));
        }
        applyModes(mode, argsVec, i);

    }
}




