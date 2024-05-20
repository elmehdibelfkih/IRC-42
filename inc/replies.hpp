#ifndef REPLIES_HPP
#define REPLIES_HPP
#include <iostream>

// ///////////////

#define ERR_NOTREGISTERED(client) "451 " + client + " :You have not registered\r\n"
#define ERR_PASSWDMISMATCH(client) "465 " + client + " :Password incorrect\r\n"
#define ERR_ALREADYREGISTERED(client) "462 " + client + " :You may not reregister\r\n"
#define ERR_NEEDMOREPARAMS(client,command) "461 " + client + " " + command + " :Not enough parameters\r\n"
#define ERR_NICKNAMEINUSE(client,nick) "433 " + client + " " + nick + " :Nickname is already in use\r\n"
#define ERR_NONICKNAMEGIVEN(client) "431 " + client +  " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(client,nick) "432 " + client + " " + nick + " :Erroneus nickname\r\n"

#define ERR_BADCHANMASK(channel) "476 " + channel + ":Bad Channel Mask\r\n"

#define ERR_TOOMANYCHANNELS(client,channel) "405" + client + " " + channel + ":You have joined too many channels\r\n"
#define ERR_BADCHANNELKEY(client,channel) "475" + client + " "  + channel + " : Key incorrect, Cannot join channel (+k)\r\n"

#endif