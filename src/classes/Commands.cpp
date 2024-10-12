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
    {
        std::cout << "kayan >>" << this->getClientByNickName(params)->getNickName() << std::endl; // hna 
        return this->_clients[i].sendMsg(ERR_NICKNAMEINUSE(nickname, this->_clients[i].getMessage().getToken()));
    }
    else
    {
        std::string msg = CHANGENICK(nickname , _clients[i].getUserName(), _clients[i].getIP(), params);
        std::map<std::string, Channel> ::iterator it = this->_channels.begin();
        std::string oldnickname = this->_clients[i].getNickName();
        this->_clients[i].setNickName(params);
        for(; it != _channels.end(); ++it)
        {
            if (it->second._clients.find(oldnickname) != it->second._clients.end()) 
            {
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
        }
        else
        {
            if (key.empty())
                this->createChannel(channelname, key, (t_Mode){false, false, false, false});
            else
                this->createChannel(channelname, key, (t_Mode){false, false, true, false});
        }
        this->_channels[channelname].addClient(&this->_clients[i]);
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

    if (argsVec.size() > 1)
        reason = params.substr(channelname.size() + 1); 
    else
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

    if (argsVec.size() > 2)
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

    if (argsVec.size() == 1)
    {
        if (this->_channels[argsVec[0]].getTopic().empty())
            return this->_clients[i].sendMsg(RPL_NOTOPIC(this->_clients[i].getNickName(), argsVec[0]));
        else
            return this->_clients[i].sendMsg(RPL_TOPIC(this->_clients[i].getNickName(), argsVec[0], this->_channels[argsVec[0]].getTopic()));
    }
    if (argsVec.size() > 1)
    {
        std::string newTopic = params.substr(argsVec[0].size() + 1);
        if (!newTopic.empty() && newTopic[0] == ':')
            newTopic = newTopic.substr(1);

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
    if (target.at(0) == '#')
    {
        if (findChannelName(target) == false)
            return this->_clients[i].sendMsg(ERR_NOSUCHSERVER(this->_clients[i].getNickName()));
        if (!is_memberInChannel(target, this->_clients[i]))
            return this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), target));
        
        std::string msg = params.substr(target.size() + 1) ;
        this->_channels[target].broadcastMessage(":" + _clients[i].getNickName() + "!~" + _clients[i].getUserName() + "@" + _clients[i].getIP() + " PRIVMSG " + target + " " + msg + "\r\n", _clients[i]);
    }
    else
    {
        Client &sender = this->_clients[i];
        Client *cl = getClientByNickName(target);
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
    cl->sendMsg(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));
    this->_clients[i].sendMsg(RPL_INVITING(this->_clients[i].getNickName(), inviteduser, channelname));
}

void Server::applyMode(std::vector < std::pair<std::string, std::string> > modes, Channel *channel ,int i)
{
    Client &client = this->_clients[i];

    for (size_t i = 0; i < modes.size(); i++)
    {
        std::pair<std::string, std::string> mode = modes[i];
        if (mode.first == "+i" || mode.first == "-i")
        {
            std::cout << "invite only mode" << std::endl;
            inviteOnlyMode(channel, client, mode);
        }
        else if (mode.first == "+l" || mode.first == "-l")
        {

            std::cout << "user limit mode" << std::endl;
            limitMode(channel, client, mode);
        }
        else if (mode.first == "+k" || mode.first == "-k")
        {
            std::cout << "key mode" << std::endl;
            keyMode(channel, client, mode);
        }
        else if (mode.first == "+o" || mode.first == "-o")
        {
            std::cout << "set oper mode" << std::endl;
            operatorMode(channel, client, mode);
        }
        else if (mode.first == "+t" || mode.first == "-t")
        {
            std::cout << "topic restric mode" << std::endl;
            topicMode(channel, client, mode);
        }
        else
            client.sendMsg(ERR_UNKNOWNMODE(client.getNickName(), mode.first));
    }
    std::cout << "apply mode" << std::endl;
}


void Server::modeCommand(int i)
{
    std::string params;
    std::vector < std::pair<std::string, std::string> > modes;
    bool syntaxerror = false;

    if ((params = this->_clients[i].getMessage().getToken()).empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "MODE")));
    std::vector<std::string> modeArguments = splitString(params, ' ');
    std::string channelName = modeArguments[0];
    modeArguments.erase(modeArguments.begin());
    modes = parseModeArgs(modeArguments, syntaxerror);
    if (!channelName.empty() && channelName.at(0) == '#')
    {
        if (!modeArguments.size())
        {
            std::string key = (_channels[channelName].getMode('k') ? _channels[channelName].getpassWord() : "");
            std::string userlimit = "";
            if(_channels[channelName].getMode('l'))
                userlimit = intToString(_channels[channelName].getUserlimit());
            return (this->_clients[i].sendMsg(RPL_CHANNELMODEIS(this->_clients[i].getNickName(), _channels[channelName].getChannelName(), _channels[channelName].showModes(), key, userlimit) ));
        }
        if (!findChannelName(channelName))
            return (this->_clients[i].sendMsg(ERR_NOSUCHCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!is_memberInChannel(channelName, this->_clients[i]))
            return (this->_clients[i].sendMsg(ERR_NOTONCHANNEL(this->_clients[i].getNickName(), channelName)));
        if (!this->_channels[channelName].isOperator(_clients[i]))
            return (this->_clients[i].sendMsg(ERR_CHANOPRIVSNEEDED(this->_clients[i].getNickName(), channelName)));
    }
    if (modeArguments.empty())
        return (this->_clients[i].sendMsg(ERR_NEEDMOREPARAMS(this->_clients[i].getNickName(), "MODE")));
    applyMode(modes, &_channels[channelName], i);
}



std::vector < std::pair<std::string, std::string> > parseModeArgs(std::vector<std::string> modeArgse, bool &syntaxerror)
{
    std::vector < std::pair<std::string, std::string> > ret;
    bool plus;

    if (modeArgse.empty())
    {
        syntaxerror = true;
        return ret;
    }
    std::string modes = modeArgse[0];
    modeArgse.erase(modeArgse.begin());
    if (modes.size() < 2 || (modes[0] != '+' && modes[0] != '-') || !isValidmode(modes))
        syntaxerror = true;
    for (size_t i = 0; i < modes.size(); i++)
    {
        if (modes[i] == '+')
            plus = true;
        else if (modes[i] == '-')
            plus = false;
        else if (modes[i] == 'i')
            ret.push_back(std::make_pair(plus ? "+i" : "-i", ""));
        else if (modes[i] == 't')
            ret.push_back(std::make_pair(plus ? "+t" : "-t", ""));
        else if (modes[i] == 'k')
        {
            if (plus && !modeArgse.empty())
            {
                ret.push_back(std::make_pair("+k", modeArgse[0]));
                modeArgse.erase(modeArgse.begin());
            }
            else
                ret.push_back(std::make_pair("-k", ""));
        }
        else if (modes[i] == 'l')
        {
            if (plus && !modeArgse.empty())
            {
                ret.push_back(std::make_pair("+l", modeArgse[0]));
                modeArgse.erase(modeArgse.begin());
            }
            else
                ret.push_back(std::make_pair("-l", ""));
        }
        else if (modes[i] == 'o' && !modeArgse.empty())
        {
            ret.push_back(std::make_pair(plus ? "+o" : "-o", modeArgse[0]));
            modeArgse.erase(modeArgse.begin());
        }
    }
    return ret;
}


void  inviteOnlyMode(Channel *channel, Client &client, std::pair<std::string, std::string> mode)
{
    if (mode.first == "+i")
    {
        channel->setInviteOnly(true);
        channel->broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channel->getChannelName() + " +i :Invite-only mode enabled" + "\r\n");
    }
    else
    {
        channel->setInviteOnly(false);
        channel->broadcastMessage(":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getIP() + " MODE " + channel->getChannelName() + " -i :Invite-only mode disabled" + "\r\n");
    }
}

void  limitMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode)
{
    if (mode.first == "+l")
    {
        int limit = std::atoi(mode.second.c_str());
        if (limit <= 0 || limit > 1024)
            return cli.sendMsg(ERR_SYNTAXERROR(cli.getNickName(), "MODE +l"));
        channel->setUserLimit(limit);
        channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP() + " MODE " + channel->getChannelName() + " +l :User limit set to " + intToString(limit) + "\r\n");
    }
    else
    {
        channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP()  +" MODE " + channel->getChannelName() + " -l :User limit removed " + "\r\n");
    }
}

void  keyMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode)
{
    if (mode.first == "+k")
    {
        channel->setpassWord(mode.second);
        channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP() + " MODE " + channel->getChannelName() + " +k :Channel key set" + "\r\n");
    }
    else
    {
        channel->setModes('k', false);
        channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP()  + " MODE " + channel->getChannelName() + " -k :Channel key removed" + "\r\n");
    }
}

void  operatorMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode)
{
    bool signal = (mode.first[0] == '+');
    Client *cl = channel->getClientByNickName(mode.second);

    if (cl == NULL)
        return cli.sendMsg(ERR_NOSUCHNICK(cli.getNickName()));
    if (mode.first == "+o")
    {
        channel->setOperMode(*cl, true);
        channel->refrechChannel(cli);
    }
    else
    {
        channel->setOperMode(*cl, false);
        channel->refrechChannel(cli);
    }
    channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP()  + " MODE " + channel->getChannelName() + " " + (signal ? "+o " : "-o ") +  mode.second + "\r\n");

}

void  topicMode(Channel *channel, Client &cli, std::pair<std::string, std::string> mode)
{
    if (mode.first == "+t")
    {
        channel->setTopicRestricted(true);
        channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP() + " MODE " + channel->getChannelName() + " +t :Topic restriction enabled" + "\r\n");
    }
    else
    {
        channel->setTopicRestricted(false);
        channel->broadcastMessage(":" + cli.getNickName() + "!" + cli.getUserName() + "@" + cli.getIP() + " MODE " + channel->getChannelName() + " -t :Topic restriction disabled" + "\r\n");
    }
}

