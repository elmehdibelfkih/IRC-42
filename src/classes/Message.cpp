/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:19 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/16 06:47:40 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Message.hpp"

Message::Message()
{
    this->_buffer = "";
    this->_fdsender = -1;
    this->_isReady = false;
}

Message& Message::operator=(const Message& obj)
{
    if (this != &obj)
    {
        this->_buffer = obj._buffer;
        this->_fdsender = obj._fdsender;
        this->_isReady = obj._isReady;
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
    if (this->_buffer.find('\n'))
        this->_isReady = true;
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
    if (msg.getBuffer().find('\n'))
        this->_isReady = true;
    this->_buffer.append(msg.getBuffer());
}

bool Message::getIsReady() const
{
    return this->_isReady;
}

void Message::setBuffer(std::string str)
{
    this->_buffer = str;
}

void Message::setIsReady(bool b)
{
    this->_isReady = b;
}

void Message::clearBuffer()
{
    this->_buffer = "";
    this->_isReady = false;
}