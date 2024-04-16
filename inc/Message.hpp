/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebelfkih <ebelfkih@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 17:46:53 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/04/16 05:56:41 by ebelfkih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <iostream>

class Message
{
private:
    std::string _buffer;
    int         _fdsender;
    bool        _isReady;
public:
    Message();
    Message& operator=(const Message& obj);
    Message(const Message& obj);
    ~Message();

    // getters
    std::string getBuffer()const;
    bool getIsReady() const;
    
    
    // setters
    void setBuffer(std::string str);
    void setIsReady(bool b);
    

    Message(std::string buffer, int sender);
    Message& operator+(const std::string& str);
    void myAppend(Message msg);
    void clearBuffer();
};

#endif
