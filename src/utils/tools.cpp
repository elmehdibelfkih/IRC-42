/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 21:47:11 by ybouchra          #+#    #+#             */
/*   Updated: 2024/09/07 13:32:09 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"

std::string join_msg(std::vector <std::string>&vec)
{
    std::string res = "";
    size_t i = 0;
    while(vec.size() > ++i)
        res += vec[i] + " ";
    return res;
} 

std::string trimFunc(std::string &str)
{
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
