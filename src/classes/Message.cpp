/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:19 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/19 20:52:05 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Message.hpp"

Message::Message()
{
    this->_buffer = "";
    this->_fdsender = -1;
}

Message& Message::operator=(const Message& obj)
{
    if (this != &obj)
    {
        this->_buffer = obj._buffer;
        this->_fdsender = obj._fdsender;
    }
    return *this;
}

Message::Message(const Message& obj)
{
    *this = obj;
}

Message::~Message()
{
    this->_buffer.clear();
}

////////////////////////////////////////////////////////////////////////////////////

Message::Message(std::string buffer, int sender) : _buffer(buffer), _fdsender(sender)
{

}

Message& Message::operator+(const std::string& str)
{
    this->_buffer.append(str);
    return *this;
}

std::string Message::getBuffer()const
{
    return this->_buffer;
}

void Message::myAppend(Message msg)
{
    this->_buffer.append(msg.getBuffer());
}

bool Message::IsReady()
{
    if (strchr(this->_buffer.c_str(), '\n'))
    {
        this->parsBuffer();
        return true;
    }
    return false;
}

void Message::setBuffer(std::string str)
{
    this->_buffer = str;
}


void Message::clearBuffer()
{
    this->_buffer = "";
}

void Message::parsBuffer()
{   
    this->_tokens.clear();
    std::string::size_type start = 0;
    std::string::size_type end = this->_buffer.find(' ');
    std::string tmp;

    while (end != std::string::npos) {
        tmp = this->_buffer.substr(start, end - start);
        // if(tmp.find('\n') == std::string::npos)
        //     tmp.erase(tmp.find('\n'), 1);
        this->_tokens.push_back(tmp);
        start = end + 1;
        end = this->_buffer.find(' ', start);
    }
    tmp = this->_buffer.substr(start);
    if(tmp.find('\n') != std::string::npos)
            tmp.erase(tmp.find('\n'), 1);
    this->_tokens.push_back(tmp);
}
