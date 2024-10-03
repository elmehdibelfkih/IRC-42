/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ussef <ussef@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 21:47:11 by ybouchra          #+#    #+#             */
/*   Updated: 2024/09/30 08:03:15 by ussef            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"



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

int stringToInt(const std::string& str) {
    std::stringstream ss(str);
    long long temp;  // Use long long to detect overflow
    ss >> temp;

    if (ss.fail() || temp < std::numeric_limits<int>::min() || temp > std::numeric_limits<int>::max()) {
        return -1;  // Return 0 to indicate overflow or conversion error
    }

    return static_cast<int>(temp);  // Safe to cast to int
}