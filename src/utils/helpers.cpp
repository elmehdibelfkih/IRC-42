/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 20:45:46 by slaanani          #+#    #+#             */
/*   Updated: 2024/07/14 02:21:25 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"

void	printHeader()
{
	std::cout << "  __ _       _          " << std::endl;
	std::cout << " / _| |_    (_)_ __ ___ " << std::endl;
	std::cout << "| |_| __|   | | '__/ __|" << std::endl;
	std::cout << "|  _| |_    | | | | (__ " << std::endl;
	std::cout << "|_|  \\__|___|_|_|  \\___|" << std::endl;
	std::cout << "       |_____|          " << std::endl;
}

void	printUsage()
{
	std::cout << "Usage: ./ft_irc <port> <password>" << std::endl;
}
