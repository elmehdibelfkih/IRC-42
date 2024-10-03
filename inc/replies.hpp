#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <iostream>



//Error Replies (ERR)
#define ERR_NOTREGISTERED(client) (SERVERNAME + " 451 " + client + " :You have not registered\r\n")
#define ERR_PASSWDMISMATCH(client) (SERVERNAME + " 464 " + client + " :Password incorrect\r\n")
#define ERR_ALREADYREGISTERED(client) (SERVERNAME + " 462 " + client + " :You may not reregister\r\n")

#define ERR_NICKNAMEINUSE(client, nick) (SERVERNAME + " 433 " + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NONICKNAMEGIVEN(client) (SERVERNAME + " 431 " + client + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(client, nick) (SERVERNAME + " 432 " + client + " " + nick + " :Erroneous nickname\r\n")
#define ERR_ERRONEUSUSERNAME(client, user) (SERVERNAME + " 432 " + client + " " + user + " :Erroneous username\r\n")
#define ERR_NEEDMOREPARAMS(client, command) (SERVERNAME + " 461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_NOSUCHCHANNEL(client, channel) (SERVERNAME + " 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_TOOMANYCHANNELS(client, channel) (SERVERNAME + " 405 " + client + " " + channel + " :You have joined too many channels\r\n")
#define ERR_BANNEDFROMCHAN(client, channel) (SERVERNAME + " 474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_BADCHANNELKEY(client, channel) (SERVERNAME + " 475 " + client + " " + channel + " :Key incorrect, cannot join the channel (+k)\r\n")
#define ERR_CHANNELISFULL(client, channel) (SERVERNAME + " 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel) (SERVERNAME + " 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(client, channel) (SERVERNAME + " 476 " + client + " " + channel + " :Bad channel name\r\n")

#define ERR_USERONCHANNEL(client, channel) (SERVERNAME + " 443 " + client + " " + channel + " :is already on channel\r\n")
#define ERR_SYNTAXERROR(client, command) (SERVERNAME + " " + client + " " + command + " :Invalid parameters provided\r\n")
#define ERR_NOTONCHANNEL(client, channel) (SERVERNAME + " 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) (SERVERNAME + " 482 " + client + " " + channel + " :You're not channel operator\r\n")
#define ERR_NOSUCHNICK(client) (SERVERNAME + " 401 " + client + " :No such nick\r\n")
#define ERR_NOSUCHSERVER(client) (SERVERNAME + " 402 " + client + " :No such channel\r\n")
#define ERR_NOTEXTTOSEND(client) (SERVERNAME + " 412 " + client + " :No text to send\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel) (SERVERNAME + " 404 " + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOORIGIN(client) (SERVERNAME + " 409 " + client + " :No origin specified\r\n")
#define ERR_UNKNOWNMODE(client, modechar) (SERVERNAME + " 472 " + client + " " + modechar + " :Unknown mode\r\n")
#define ERR_UNKNOWNCOMMAND(nick) (SERVERNAME + " 421 " + nick + " " + " :Unknown command\r\n")
#define RPL_CREATIONTIME(client, channel, creation_time) (SERVERNAME + " 329 " + client + " " + channel + " " + creation_time + "\r\n")


// Reply Messages (RPL)


#define RPL_WELCOME(nick, user, host) (SERVERNAME + " 001 " + nick + " :*** Welcome to the IRC Network " + nick + "!" + user + "@" + host + "\r\n")
#define RPL_YOURHOST(nick) (SERVERNAME + " 002 " + nick + " :*** Your host is ircserver@localhost.io , running version v2.00\r\n")
#define RPL_CREATED(nick, date) (SERVERNAME + " 003 " + nick + " :*** This server was created " + date + "\r\n")
#define RPL_JOIN(nick, user, host, channel) (":" + nick + "!~" + user + "@" + host + " JOIN " + channel + "\r\n")
#define RPL_PART(nick, user, host, channel, reason) (":" + nick + "!~" + user + "@" + host + " PART " + channel + " " + reason + "\r\n")
#define RPL_KICK(nick, user, host, channel, kickeduser, reason) (":" + nick + "!~" + user + "@" + host + " KICK " + channel + " " + kickeduser + " " + reason + "\r\n")

#define RPL_INVITING(client, inviteduser, channel) (SERVERNAME + " 341 " + client + " :" + inviteduser + " has been invited to join " + channel + "\r\n")
#define RPL_KICKEDUSER(nick, user, host, channel, kickeduser, raison) (SERVERNAME + " " + nick + "!" + user + "@" + host + " KICK " + channel + " " + kickeduser + " " + raison + "\r\n")



#define RPL_NOTOPIC(client, channel) (SERVERNAME + " 331 " + client + " " + channel + " :No topic to set.\r\n")
#define RPL_TOPIC(client, channel, topic) (SERVERNAME + " 332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_TOPICWHOTIME(client, channel, nick, _time) (SERVERNAME + " 333 " + client + " " + channel + " " + nick + " " + _time + "\r\n")


#define RPL_LIST(client, channel, userCount, topic) (SERVERNAME + " 322 " + client + " " + channel + " " + std::to_string(userCount) + " :" + topic + "\r\n")
#define RPL_LISTEND(client) (SERVERNAME + " 323 " + client + " :End of /LIST\r\n")
#define RPL_NAMREPLY(client, symbol, channel, userlist) (SERVERNAME + " 353 " + client + " = " + channel + " :" + userlist + "\r\n")
#define RPL_ENDOFNAMES(client, channel) (SERVERNAME + " 366 " + client + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_AWAY(client, msg) (SERVERNAME + " 301 " + client + " :is currently away and sends the away message: " + msg + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, modestring, key, userlimit) (SERVERNAME + " 324 " + client + " " + channel + " :" + modestring + " " + key + " " + userlimit + "\r\n")

#define RPL_VALIDNICK() (SERVERNAME + " :Nickname accepted\r\n")
#define CHANGENICK(nick, username, host, message) ":" + (nick) + "!" + (username) + "@" + (host) + " NICK :" + (message) + "\r\n"
#define RPL_VALIDPASS() (SERVERNAME + " :Password accepted | Continue with NICK and USER\r\n")


#endif