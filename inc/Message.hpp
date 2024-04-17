/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:53 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/17 04:06:45 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <iostream>
#include <cstring>

class Message
{
private:
    std::string _buffer;
    int         _fdsender;
public:
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
    bool IsReady() const;
};

#endif
