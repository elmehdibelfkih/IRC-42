/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 20:45:46 by slaanani          #+#    #+#             */
/*   Updated: 2024/09/29 11:06:03 by ebelfkih         ###   ########.fr       */
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

std::string trimFunc(std::string &str)
{
    if(str.empty())
        return "";
    std::size_t start = 0;
    while(str.size() > start && std::isspace(str[start]))
        start++;

    std::size_t end = str.size();
    while(end > start && std::isspace(str[end - 1]))
        end--;
    
    return str.substr(start, end - start) ;

}
std::vector<std::string> splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> vec ;
    std::stringstream iss(str);
    std::string buff;

    if(str.empty())
        return vec;
    while (std::getline(iss, buff, delimiter)) {
        if(!buff.empty())
            vec.push_back(trimFunc(buff));
    }
    return vec;
}

std::string intToString(int num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

std::string getCurrentTime() {
    // Get the current time
    std::time_t rawTime;
    std::time(&rawTime);

    // Convert it to a struct tm, which contains time components
    struct tm *timeInfo = std::localtime(&rawTime);

    // Buffer to hold the formatted date and time
    char buffer[80];

    // Format: Day Month Date HH:MM:SS Year (like Wed Sep 27 15:30:45 2024)
    std::strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", timeInfo);

    // Return as a string
    return std::string(buffer);
}

void logToFile(const std::string& message)
{
    std::ofstream outFile("logfile.log", std::ios::app); // Open in append mode
    if (outFile.is_open()) {
        outFile << message;
        outFile.close();
    } else {
        std::cerr << "Error opening log file!" << std::endl;
    }
}

bool isValidmode(const std::string& str)
{
    std::string allowedChars = "+-itkol";
    
    for (std::string::size_type i = 0; i < str.length(); ++i) 
    {
        if (allowedChars.find(str[i]) == std::string::npos) {
            return false;
        }
    }
    
    return true;
}