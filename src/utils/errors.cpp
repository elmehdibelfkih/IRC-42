/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 20:59:12 by slaanani          #+#    #+#             */
/*   Updated: 2024/07/17 05:34:33 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.hpp"

void printErrorAndExit(int errorCode)
{
	std::string errorMessage;
	
	errorMessage = getErrorString(errorCode);
	std::cerr << "Error: Code " << errorCode << " - " << errorMessage << std::endl;
	exit(errorCode);
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