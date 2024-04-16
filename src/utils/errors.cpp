/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaanani <slaanani@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 20:59:12 by slaanani          #+#    #+#             */
/*   Updated: 2024/04/06 22:24:04 by slaanani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.hpp"

void printErrorAndExit(int errorCode)
{
	std::string errorMessage;
	
	errorMessage = getErrorString(errorCode);
	std::cerr << "Error: Code " << errorCode << " - " << errorMessage << std::endl;
	std::exit(errorCode);
}

std::string getErrorString(int errorCode)
{
	std::stringstream ss;
	std::string errorCodeString;
	std::map<int, std::string> errorMap;

	ss << errorCode;
	errorCodeString = ss.str();
	errorMap.insert(std::make_pair(INVALID_ARGUMENT_ERROR, "Invalid argument provided"));
	if (errorMap.count(errorCode) > 0)
		return errorMap[errorCode];
	else
		return "Unknown Error (Code: " + errorCodeString + ")";
}