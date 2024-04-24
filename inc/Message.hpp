/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:53 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/24 16:53:23 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <iostream>
#include <cstring>
#include <vector>
#include "IRC.hpp"


class Message
{
private:
    std::string     _buffer; // mehdi
    std::string     _tokens; // <==
    int             _fdsender;
    int             _command; // <==
public:
    Message();
    Message& operator=(const Message& obj);
    Message(const Message& obj);
    ~Message();

    // getters
    std::string getBuffer()const;
    int getCommand()const;
    std::string getToken()const;
    
    
    // setters
    void setBuffer(std::string str);   
    void setCommand(int cmd);   
    // void setToken()const;

    Message(std::string buffer, int sender);
    Message& operator+(const std::string& str);
    void myAppend(Message msg);
    void clearBuffer();
    bool IsReady();
    void parsBuffer();
};

#endif
