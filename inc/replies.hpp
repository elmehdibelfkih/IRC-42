#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <iostream>

// ///////////////

#define ERR_NOTREGISTERED(client) "451 " + client + " :You have not registered\r\n"
#define ERR_PASSWDMISMATCH(client) "465 " + client + " :Password incorrect\r\n"
#define ERR_ALREADYREGISTERED(client) "462 " + client + " :You may not reregister\r\n"
#define ERR_NICKNAMEINUSE(client,nick) "433 " + client + " " + nick + " :Nickname is already in use\r\n"
#define ERR_NONICKNAMEGIVEN(client) "431 " + client +  " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(client,nick) "432 " + client + " " + nick + " :Erroneus nickname\r\n"


#define ERR_NEEDMOREPARAMS(client, command) ("461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_NOSUCHCHANNEL(client, channel) ("403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_TOOMANYCHANNELS(client, channel) ("405 " + client + " " + channel + " :You have joined too many channels\r\n")
#define ERR_BADCHANNELKEY(client, channel) ("475 " + client + " " + channel + " :Key incorrect, Cannot join channel (+k)\r\n")
#define ERR_BANNEDFROMCHAN(client, channel) ("474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_CHANNELISFULL(client, channel) ("471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel) ("473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(client, channel) ("476 " + client + " " + channel + " :Bad Channel Name\r\n")
#define ERR_USERONCHANNEL(client, channel) ("443 " + client + " " + channel + " :is already on channel\r\n")
#define ERR_SYNTAXERROR(client, command) ("461 " + client + " " + command + " : SyntaxErr \r\n")
#define ERR_NOTONCHANNEL(client, channel) ("442 " + client + " " + channel + " :You're not on that channel.\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) ("482 " + client + " " + channel + " :You're not channel operator\r\n")


#define RPL_TOPIC(client, channel, topic) ("332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_TOPICWHOTIME(client, channel, nick, time) ("333 " + client + " " + channel + " " + nick + " " +time + "\r\n")
#define RPL_NOTOPIC(client, channel) ("331 " + client + " " + channel + " :No topic to set.\r\n")
#define RPL_ENDOFNAMES(client, channel) ("366 " + client + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_SUCCESS(nick, user, host, channel, raison) ( nick + "!" + user + "@" + host + " PART " + channel + " :" + nick + " is leaving the channel " + channel+ " : " + raison + ".\r\n")
//.....
#define RPL_NAMREPLY(client, symbol, channel, nick) ("353 " + client + " " + symbol + " " + channel + " :@" + nick + "\r\n")

//prvmsg
#define RPL_AWAY(client, msg) ("301" + client + " is currently away and  and sends the away message: " + msg +  "\r\n")

#define ERR_NOSUCHNICK(client, nick) ("401 " + client + " " + nick  + " :No such nick\r\n")
#define ERR_NOSUCHSERVER(client, server) ("402 " + client + " " + server  + " :No such channel\r\n")
#define ERR_NOTEXTTOSEND(client) ("412 " + client +  " :No text to send\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel) ("404 " + client + " " + channel + " :Cannot send to channel\r\n")

#endif