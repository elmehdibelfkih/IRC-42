/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/21 09:49:55 by ybouchra          #+#    #+#             */
/*   Updated: 2024/09/29 11:44:11 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <ctime>
#include "errors.hpp"
#include <cstdio>
#include <string>
#include <fstream>
// #include "Channel.hpp"

enum commands
{
    PASS, // Used to set a password when connecting to the server.
    NICK, // Sets the user's nickname.
    USER, // Sets the username, hostname, servername, and realname of a user.
    JOIN, // Joins a channel or channels. >> JOIN #channel1,#channel2,#channel3
    PART, // Leaves a channel or channels.
    PRIVMSG, // Sends a message to a user or channel.
    NOTICE, // Sends a notice to a user or channel.
    TOPIC, // Sets the topic for a channel.
    AWAY, // Sets or unsets the user's away status.
    MODE, // Sets channel modes or user modes. >> MODE #channel +o user
    QUIT, // Disconnects from the server.
    WHO, // Retrieves information about users matching a given mask.
    NAMES, // Retrieves a list of users in a channel.
    LIST, // Lists all channels on the server.
    INVITE, // Invites a user to join a channel.
    KICK, // Kicks a user from a channel.
    OPER, // Authenticates as an IRC operator.
    KILL, // Disconnects a user from the server.
    PING, // Checks if the server is still alive.
    PONG, // Responds to a PING message from the server.
    UNKNOWN // unknown command.
};

#define ES_GREEN "\033[32m"
#define ES_RESET "\033[0m"
#define TOPICLEN 390
#define USERLEN 17
#define NICKLEN 17
#define CHANNELNAMELEN 18
#define LIMITCHANNELS  11

#define SERVERNAME std::string(":ircserver")

void		printHeader();
void		printUsage();
std::string trimFunc(std::string& str);
std::vector<std::string> splitString(const std::string& str, char delimiter);
std::string join_msg(std::vector <std::string>&vec);
std::string intToString(int num);
std::string getCurrentTime();
void logToFile(const std::string& message);
bool isValidmode(const std::string& str);
// t_Mode  parseModeArgs(std::string &params, bool &syntaxerror);


#endif


/**/

// JOIN <channel> [<key>]

//PART <channel> [<message>]

// INVITE <nickname> <channel>

// KICK <channel> <nickname> [<comment>]

// TOPIC <channel> [<topic>]

// PRIVMSG <target> <message>

// USER <username>++

// NICK <nickname>

//  PASS <password>

// MODE <channelname> <modes> <..>


// For Channels: 
// MODE <channel> +/-<modes> [<parameters>]
// Supported modes: +/-ioktl

// i: Invite-only channel
// o: Channel operator
// k: Channel key
// t: Topic settable by channel operator only
// l: Set user limit
/***/