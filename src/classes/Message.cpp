/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:19 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/17 04:20:39 by ebelfkih         ###   ########.fr       */
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

bool Message::IsReady() const
{
    if (strchr(this->_buffer.c_str(), '\n'))
        return true;
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