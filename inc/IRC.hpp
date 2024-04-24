/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 20:48:09 by slaanani          #+#    #+#             */
/*   Updated: 2024/04/24 16:41:08 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <map>
#include <sstream>
#include "errors.hpp"

enum commands
{
    PASS, // Used to set a password when connecting to the server.
    NICK, // Sets the user's nickname.
    USER, // Sets the username, hostname, servername, and realname of a user.
    JOIN, // Joins a channel or channels.
    PART, // Leaves a channel or channels.
    PRIVMSG, // Sends a message to a user or channel.
    NOTICE, // Sends a notice to a user or channel.
    MODE, // Sets channel modes or user modes.
    TOPIC, // Sets the topic for a channel.
    QUIT, // Disconnects from the server.
    WHO, // Retrieves information about users matching a given mask.
    NAMES, // Retrieves a list of users in a channel.
    LIST, // Lists all channels on the server.
    INVITE, // Invites a user to join a channel.
    KICK, // Kicks a user from a channel.
    OPER, // Authenticates as an IRC operator.
    KILL, // Disconnects a user from the server.
    AWAY, // Sets or unsets the user's away status.
    PING, // Checks if the server is still alive.
    PONG, // Responds to a PING message from the server.
    UNKNOWN // unknown command.
};

#define ES_GREEN "\033[32m"
#define ES_RESET "\033[0m"

#define TOPICLEN 390
#define USERLEN 17
#define NICKLEN 17


void		printHeader();
void		printUsage();



// to do : start
// to do : start
#endif // IRC_HPP