#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <iostream>

// ///////////////


#define ERR_NOTREGISTERED(client) (SERVERNAME + "451 " + client + " :You have not registered 🛑\r\n")
#define ERR_PASSWDMISMATCH(client) (SERVERNAME + "465 " + client + " :Password incorrect 🛑\r\n")
#define ERR_ALREADYREGISTERED(client) (SERVERNAME + "462 " + client + " :You may not reregister again 🔒\r\n")
#define ERR_NICKNAMEINUSE(client,nick) (SERVERNAME + "433 " + client + " " + nick + " :Nickname is already in use 🛑\r\n")
#define ERR_NONICKNAMEGIVEN(client) (SERVERNAME + "431 " + client +  " :No nickname given 🛑\r\n")
#define ERR_ERRONEUSNICKNAME(client,nick) (SERVERNAME + "432 " + client + " " + nick + " :Erroneus nickname 🛑\r\n")
#define ERR_ERRONEUSUSERNAME(client,user) (SERVERNAME + "432 " + client + " " + user + " :Erroneus username 🛑\r\n")
#define ERR_NEEDMOREPARAMS(client, command) (SERVERNAME + "461 " + client + " " + command + " :Not enough parameters 🛑\r\n")
#define ERR_NOSUCHCHANNEL(client, channel) (SERVERNAME + "403 " + client + " " + channel + " :No such channel 🛑\r\n")
#define ERR_TOOMANYCHANNELS(client, channel) (SERVERNAME + "405 " + client + " " + channel + " :You have joined too many channels 🛑\r\n")
#define ERR_BANNEDFROMCHAN(client, channel) (SERVERNAME + "474 " + client + " " + channel + " :Cannot join channel (+b) 🛑\r\n")
#define ERR_BADCHANNELKEY(client, channel) (SERVERNAME + "475 " + client + " " + channel + " :Key incorrect, Cannot join the channel (+k) 🛑\r\n")
#define ERR_CHANNELISFULL(client, channel) (SERVERNAME + "471 " + client + " " + channel + " :Cannot join channel (+l) 🛑\r\n")
#define ERR_INVITEONLYCHAN(client, channel) (SERVERNAME + "473 " + client + " " + channel + " :Cannot join channel (+i) 🛑\r\n")
#define ERR_BADCHANMASK(client, channel) (SERVERNAME + "476 " + client + " " + channel + " :Bad Channel Name 🛑\r\n")
#define ERR_USERONCHANNEL(client ) (SERVERNAME + "443 " + client + " :" + " is already on channel 🛑\r\n")
#define ERR_SYNTAXERROR(client, command) (SERVERNAME + "461 " + client + " " + command + " : Invalid parameters provided 🛑\r\n")
#define ERR_NOTONCHANNEL(client, channel) (SERVERNAME + "442 " + client + " " + channel + " :You're not on that channel 🛑\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) (SERVERNAME + "482 " + client + " " + channel + " :You're not channel operator 🛑\r\n")

#define ERR_NOSUCHNICK(client) (SERVERNAME + "401 " + client   + " :No such nick 🛑\r\n")
#define ERR_NOSUCHSERVER(client) (SERVERNAME + "402 " + client + " :No such channel 🛑\r\n")
#define ERR_NOTEXTTOSEND(client) (SERVERNAME + "412 " + client +  " :No text to send 🛑\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel) (SERVERNAME + "404 " + client + " " + channel + " :Cannot send to channel 🛑\r\n")
#define ERR_NOORIGIN(client) (SERVERNAME + "409 " + client + " " + " :No origin specified 🛑1\r\n")
#define ERR_UNKNOWNMODE(client, modechar) (SERVERNAME + "472 " + client + " " + modechar  + " :is unknown mode 🛑\r\n")

#define ERR_UNKNOWNCOMMAND() (SERVERNAME + "421"  + " :unknown command 🛑\r\n")


#define RPL_WELCOME(nick, user, host) (SERVERNAME + "001 " + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@" + host + " 🟢\r\n")
#define RPL_TOPIC(client, channel, topic) (SERVERNAME + "332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_TOPICWHOTIME(client, channel, nick, time) (SERVERNAME + "333 " + client + " " + channel + " " + nick + " " +time + "\r\n")
#define RPL_NOTOPIC(client, channel) (SERVERNAME + "331 " + client + " " + channel + " :No topic to set.\r\n")
#define RPL_ENDOFNAMES(client, channel) (SERVERNAME + "366 " + client + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_SUCCESS(nick, user, host, channelname, raison) (SERVERNAME + nick + "!" + user + "@" + host  + " :is leaving the channel " + channelname + " " + raison + "\r\n")
#define RPL_VALIDPASS() (SERVERNAME + ":Password accepted 🟢 | Continue with NICK and USER commands. \r\n")
#define RPL_VALIDNICK() (SERVERNAME + ":Nickname accepted 🟢  \r\n")
#define RPL_NAMREPLY(client, symbol, channel, nick) (SERVERNAME + "353 " + client + " " + symbol + " " + channel + " :@" + nick + "\r\n")
#define RPL_AWAY(client, msg) (SERVERNAME + "301" + client + " is currently away and  and sends the away message: " + msg +  "\r\n")
#define RPL_INVITING(client, inviter, channel) (SERVERNAME + "341 " + client + " :" + inviter + " has been invited to " + channel + " 🟢\r\n")
#define RPL_KICKEDUSER(nick, user, host, channel, kickeduser, raison) (SERVERNAME + nick + "!" + user + "@" + host  + "KICK  from the " + channel + " " + kickeduser  + " " + raison + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, modestring, key) (SERVERNAME + "324 "  + client + " " + channel  + " : " + modestring + " " + key  + "\r\n")

#endif