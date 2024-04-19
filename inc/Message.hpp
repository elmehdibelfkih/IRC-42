/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:53 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/19 16:28:05 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <iostream>
#include <cstring>
#include <vector>


class Message
{
private:
    std::string                 _buffer;
    int                         _fdsender;
public:
    std::vector<std::string>    _tokens;
    Message();
    Message& operator=(const Message& obj);
    Message(const Message& obj);
    ~Message();

    // getters
    std::string getBuffer()const;
    
    
    // setters
    void setBuffer(std::string str);    

    Message(std::string buffer, int sender);
    Message& operator+(const std::string& str);
    void myAppend(Message msg);
    void clearBuffer();
    bool IsReady();
    void parsBuffer();
};

#endif
