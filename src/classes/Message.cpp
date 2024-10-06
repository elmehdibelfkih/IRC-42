/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:19 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/10/06 05:52:31 by ybouchra         ###   ########.fr       */
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

int Message::getCommand()const
{
    return this->_command;
}

std::string Message::getToken()const
{
    return this->_tokens;
}

void Message::myAppend(Message msg)
{
    this->_buffer.append(msg.getBuffer());
}

bool Message::IsReady()
{

    // check first for "\r\n" delemeter
    // if not exist check then for "\n"

    int i = 2;
    size_t pos = _ss.find("\r\n");
    if (pos == std::string::npos) {
        pos = _ss.find("\n");
        i--;
    }

    // check first for "\r\n" delemeter 
    if (pos == std::string::npos)
        return false;

    _buffer = _ss.substr(0, pos + i);
    _ss.erase(0, pos + i);
    this->parsBuffer();
    return true;

}

void Message::setBuffer(std::string str)
{
    this->_buffer = str;
}

void Message::setCommand(int cmd)
{
    this->_command = cmd;
}

void Message::clearBuffer()
{
    this->_buffer = "";
}
void Message::consume_buffer(const std::string& s)
    {
        _ss.append(s);

    }
void Message::parsBuffer()
{
    
    this->_tokens.clear();
    std::string::size_type start = 0;
    std::string::size_type end = this->_buffer.find(' ');
    std::string cmd;
    std::string tmp;


    // "PASS uuuu\r\nssdsdsd sjfdsf idgsfb"

    if (end != std::string::npos)
    {
        cmd = this->_buffer.substr(start, end - start);
        start = end + 1;
        tmp = this->_buffer.substr(start);
        if(tmp.find('\n') != std::string::npos)
                tmp.erase(tmp.find('\n'), 1);
        if(tmp.find('\r') != std::string::npos)
                tmp.erase(tmp.find('\r'), 1);
        this->_tokens = tmp;
    }
    else
    {
        tmp = this->_buffer;
        if(tmp.find('\n') != std::string::npos)
                tmp.erase(tmp.find('\n'), 1);
        if(tmp.find('\r') != std::string::npos)
                tmp.erase(tmp.find('\r'), 1);
        cmd = tmp;
    }
    if (cmd == "PASS" || cmd == "pass")
        this->_command = PASS;
    else if (cmd == "NICK" || cmd == "nick")
        this->_command = NICK;
    else if (cmd == "USER" || cmd == "user")
        this->_command = USER;
    else if (cmd == "JOIN" || cmd == "join")
        this->_command = JOIN;
    else if (cmd == "PART" || cmd == "part")
        this->_command = PART;
    else if (cmd == "PRIVMSG" || cmd == "privmsg")
        this->_command = PRIVMSG;
    else if (cmd == "MODE" || cmd == "mode")
        this->_command = MODE;
    else if (cmd == "TOPIC" || cmd == "topic")
        this->_command = TOPIC;
    else if (cmd == "LIST" || cmd == "list")
        this->_command = LIST;
    else if (cmd == "INVITE" || cmd == "invite")
        this->_command = INVITE;
    else if (cmd == "KICK" || cmd == "kick")
        this->_command = KICK;
    else if (cmd == "PING" || cmd == "ping")
        this->_command = PING;
    else if (cmd == "PONG" || cmd == "pong")
        this->_command = PONG;
    else
        this->_command = UNKNOWN;
}
