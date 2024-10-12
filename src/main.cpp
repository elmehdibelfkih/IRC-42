/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:17:55 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/30 14:43:40 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/IRC.hpp"
#include "../inc/Server.hpp"


int main(int argc, char const *argv[])
{
	if (argc != 3 )
	{
		printUsage();
		printErrorAndExit(INVALID_ARGUMENT_ERROR);
	}
	else
	{
		printHeader();
		std::cout << std::endl;
		std::cout << ES_GREEN << "<< ----- ft_irc ----- >>" << ES_RESET << std::endl;
		std::cout << std::endl;
		std::cout << "Port:     " << argv[1] << std::endl;
		if(argc == 3)
			std::cout <<  "Password: " << argv[2] << std::endl;
			
		Server serv(argv[1], argv[2]);
		serv.startServer();
	}
	return 0;
}