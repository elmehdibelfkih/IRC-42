/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:53 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/06 05:52:58 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <iostream>
#include <cstring>
#include <vector>
#include "IRC.hpp"
#include <sstream>

class Message
{
private:
    std::string     _buffer;
    std::string     _ss; 
    std::string     _tokens;
    int             _fdsender;
    int             _command;
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

    Message(std::string buffer, int sender);
    Message& operator+(const std::string& str);
    void myAppend(Message msg);
    void clearBuffer();
    bool IsReady();
    void consume_buffer(const std::string& s);
    void parsBuffer();


};

#endif
