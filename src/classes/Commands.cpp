#include "IRC.hpp"
#include "Server.hpp"

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
                
                it->second.broadcastMessage(msg);
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

std::pair<std::vector<std::string>, std::vector<std::string> > parseChannels(std::string params, bool &syntaxerror)
{
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty() || argsVec.size() > 2)
    {
        syntaxerror = true;
        return std::make_pair(channels, keys);
    }
    channels = splitString(argsVec[0], ',');
    if (argsVec.size() > 1)
        keys = splitString(argsVec[1], ',');
    if (keys.size() < channels.size())
        keys.resize(channels.size(), "");
    std::pair<std::vector<std::string>, std::vector<std::string> > result;
    result.first = channels;
    result.second = keys;
    return result;
}


void Server::joinCommand(int i)
{
    bool syntaxerror = false;
    std::string params = this->_clients[i].getMessage().getToken();
    if (params.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "JOIN"));
    std::pair<std::vector<std::string>, std::vector<std::string> > channels = parseChannels(params, syntaxerror);

    for (size_t j = 0; j < channels.first.size(); j++) 
    {
        std::string channelname = channels.first[j];
        std::string key = channels.second[j];
        if (syntaxerror)
            return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "JOIN"));
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
            // std::cout << channel.getMode('l') << std::endl;
        }
        else
        {
            if (key.empty())
                this->createChannel(channelname, key, (t_Mode){false, false, false, false});
            else
                this->createChannel(channelname, key, (t_Mode){false, false, true, false});
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

void Server::partCommand(int i)
{
    std::string params, reason;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PART"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "PART"));

    std::string channelname = argsVec[0];
    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));
    if (!is_memberInChannel(channelname, this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelname));

    if (argsVec.size() > 1) //  reason provided
        reason = params.substr(channelname.size() + 1); 
    else // No reason provided
        reason = "No reason provided";
    
    if (!reason.empty() && reason[0] != ':')
        reason = ":" + reason; 

    std::string partMessage = ":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PART " + channelname + " " + reason + "\r\n";
    this->_channels[channelname].broadcastMessage(partMessage);
    this->_channels[channelname].removeClient(this->_clients[i], PART);
    
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
    std::string &kickeduser = argsVec[1];

    if (!findChannelName(channelname))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelname));

    if (!this->_channels[channelname].isOperator(_clients[i]))
        return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelname));

    Client *cl = getClientByNickName(kickeduser); 
    if (cl == NULL)
        return this->_clients[i].sendMsg(ERR_NOSUCHNICK(kickeduser));

    if (!is_memberInChannel(channelname, *cl))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(kickeduser, channelname));

    if (argsVec.size() > 2)  // reason is provided
        reason = params.substr(channelname.size() + kickeduser.size() + 2 ); 
    else
        reason = "No reason provided";
       
    if (!reason.empty() && reason[0] != ':')
        reason = ":" + reason; 

    this->_channels[channelname].broadcastMessage( RPL_KICK(_clients[i].getNickName(),_clients[i].getUserName(),_clients[i].getIP(),channelname,kickeduser,reason));
    this->_channels[channelname].removeClient(*cl, KICK);
}


void Server::topicCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "TOPIC"));

    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty())
        return this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "TOPIC"));

    if (!findChannelName(argsVec[0]))
        return this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), argsVec[0]));

    if (!is_memberInChannel(argsVec[0], this->_clients[i]))
        return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), argsVec[0]));

    if (this->_channels[argsVec[0]].getMode('t') == true)
    {
        if (!this->_channels[argsVec[0]].isOperator(_clients[i]))
            return this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), argsVec[0]));
    }

    if (argsVec.size() == 1)// display current topic
    {
        if (this->_channels[argsVec[0]].getTopic().empty())
            return this->_clients[i].sendMsg(RPL_NOTOPIC(this->_clients[i].getNickName(), argsVec[0]));
        else
            return this->_clients[i].sendMsg(RPL_TOPIC(this->_clients[i].getNickName(), argsVec[0], this->_channels[argsVec[0]].getTopic()));
    }
    // topic is provided
    if (argsVec.size() > 1)
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
        this->_channels[target].broadcastMessage(":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PRIVMSG " + target + " " + msg + "\r\n", _clients[i]);
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

    if (this->_channels[channelname].getMode('i') && !this->_channels[channelname].isOperator(_clients[i]))
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


void Server::applyMode(const std::vector<std::string> &argsVec, int i)
{
    std::string channelName = argsVec[0];
    std::string mode = argsVec[1];
    Channel &channel = this->_channels[channelName];
    Client &client = this->_clients[i];
    Client *targetClient = NULL;

    if (!channel.isOperator(client))
       return client.sendMsg(ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));

    bool signal = (mode[0] == '+');
    char modeType = mode[1];


    switch (modeType)
    {
    case 'i':
        channel.setInviteOnly(signal);
        channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " " + "i" + " :Invite-only mode " + (signal ? "enabled" : "disabled") + "\r\n");
        break;
    case 't':
        channel.setTopicRestricted(signal);
        channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " " + "t" + " :Topic restriction " + (signal ? "enabled" : "disabled") + "\r\n");
        break;
    case 'k':
        if (signal)
        {
            if (argsVec.size() < 3)
                return (client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE +k")));
            std::string key = argsVec[2];
            channel._mode.k = true;
            channel.setpassWord(key);
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " +k :Channel key set" + "\r\n");
        }
        else
        {
            channel._passWord.clear();
            channel._mode.k = false;
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  + " MODE " + channelName + " -k :Channel key removed" + "\r\n");
        }
        break;
    case 'l':
        if (signal)
        {
            if (argsVec.size() < 3)
                return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE +l"));

            int limit = std::atoi(argsVec[2].c_str());
            if (limit <= 0)
                return client.sendMsg(ERR_SYNTAXERROR(client.getNickName(), "MODE +l"));
            channel.setUserLimit(limit);
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channelName + " +l :User limit set to " + intToString(limit) + "\r\n");
        }
        else
        {
            channel._mode.l = false;
            channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  +" MODE " + channelName + " -l :User limit removed " + "\r\n");
        }
        break;
    case 'o':
        if (argsVec.size() < 3)
            return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickName(), " MODE +o/-o"));

        targetClient = getClientByNickName(argsVec[2]);
        if (targetClient == NULL)
            return (client.sendMsg(ERR_NOSUCHNICK(argsVec[2])));

        if (signal)
        {
            channel.setOperMode(*targetClient, true);
            // channel._clients[targetClient->getNickName()]->setOperStatus(true);
            channel.refrechChannel(*targetClient);
        }
        else
        {
            channel.setOperMode(*targetClient, false);
            // channel._clients[targetClient->getNickName()]->setOperStatus(false);
            channel.refrechChannel(*targetClient);
        }
        channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  + " MODE " + channelName + " " + (signal ? "+o " : "-o ") +  argsVec[2] + "\r\n");
        // channel.broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP()  + " MODE " + channelName + " " + "o" +
        //     " :Operator privileges " + (signal ? "granted to " : "removed from ") + argsVec[2] + "\r\n");
        break;
    default:
        client.sendMsg(ERR_UNKNOWNMODE(client.getNickName(), mode));
        break;
    }
}

void Server::modeCommand(int i)
{
    std::string params;
    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "MODE")));
        
    std::vector<std::string> argsVec = splitString(params, ' ');
    if (argsVec.empty() || argsVec.size() > 3)
        return (this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "MODE")));

    std::string &channelName = argsVec[0];
    if (!channelName.empty() && channelName.at(0) == '#')
    {
        if (!findChannelName(channelName))
            return (this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!is_memberInChannel(channelName, this->_clients[i]))
            return (this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!this->_channels[channelName].isOperator(_clients[i]))
            return (this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelName)));

        if (argsVec.size() == 1) // display modes
        {
            std::string key = (_channels[channelName].getMode('k') ? _channels[channelName].getpassWord() : "");
            std::string userlimit = "";
            if(_channels[channelName].getMode('l'))
                 userlimit = intToString(_channels[channelName].getUserlimit());

            return (this->_clients[i].sendMsg(RPL_CHANNELMODEIS(this->_clients[i].getNickName(), _channels[channelName].getChannelName(), _channels[argsVec[0]].showModes(), key, userlimit) ));
        }
        std::string &mode = argsVec[1];
        if (mode.empty() || (mode.at(0) != '+' && mode.at(0) != '-')   )
            return this->_clients[i].sendMsg(ERR_SYNTAXERROR(this->_clients[i].getNickName(), "MODE"));
        applyMode(argsVec, i);
    }
}

t_Mode  parseModeArgs(std::string &params, bool &syntaxerror)
{
    t_Mode mode = {false, false, false, false};
    bool plus;
    

    if (params.size() < 2 || (params[0] != '+' && params[0] != '-') || !isValidmode(params))
    {
        syntaxerror = true;
        return mode;
    }

    for (size_t i = 1; i < params.size(); i++)
    {
        if (params[i] == '+')
        {
            plus = true;
            continue;
        }
        if (params[i] == '-')
        {
            plus = false;
            continue;
        }
        if (params[i] == 'i')
            mode.i = plus;
        else if (params[i] == 'k')
            mode.k = plus;
        else if (params[i] == 't')
            mode.t = plus;
        else if (params[i] == 'l')
            mode.l = plus;
    }
    return mode;
}
