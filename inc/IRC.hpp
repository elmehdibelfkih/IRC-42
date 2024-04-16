/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 20:48:09 by slaanani          #+#    #+#             */
/*   Updated: 2024/04/14 06:26:21 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include <cstring>
#include <cstdio>
#include <vector>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <string>
#include <sstream>
#include "errors.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"



#define ES_GREEN "\033[32m"
#define ES_RESET "\033[0m"
//#define SERVER_PORT 6667 // 1024 49151

void		printHeader();
void		printUsage();



// to do : start
bool checkRangeOfPorts(int port);
// to do : start
#endif // IRC_HPP