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


#endif