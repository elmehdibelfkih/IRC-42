/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouchra <ybouchra@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 20:18:23 by ebelfkih          #+#    #+#             */
/*   Updated: 2024/09/26 09:09:16 by ybouchra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include "Message.hpp"
#include "Channel.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>



class Channel;
class Client
{
private:
    int 					_clientFdSocket;
    int                     _nbrchannels;
    bool					_authenticate;
    bool                    _pass;
    bool                    _operStatus;
    std::string				_userName;
    std::string				_nickName;
    std::string				_IP;
    Message					_msg;
    std::string             stream;
    
public:
    Client();
    Client(int clientFdSocket, bool authenticate);
    Client(const Client& obj);
    Client& operator=(const Client& obj);
    ~Client();

    
    // getters
    int         getClientFdSocket() const;
    bool        getAuthenticate() const;
    bool        getPass() const;
    bool        getOperStatus() const;
    std::string getNickName() const;
    std::string getUserName() const;
    std::string getIP() const;
    Message&    getMessage();                  
    std::string getTime() const;
    int         getnbrChannels();


    void writeMessageToSocket() {
        while (!stream.empty()) {
            ssize_t n = send(this->_clientFdSocket, stream.c_str(), stream.size(), 0);
            
            if (n == -1) {
                std::cerr << "Failed to send message to client" << std::endl;
                break;  // Handle error appropriately, possibly with retry logic
            }
            
            
            if (n < (ssize_t)stream.size()) {
                stream.erase(0, n);  // Erase the sent portion
            } else {
                stream.clear();  // Clear the stream if all data is sent
            }
        }
    }
    
    
    // setters
    void setClientFdSocket(int fd);
    void setAuthenticate(bool au);
    void setOperStatus(bool status);
    void setUserName(std::string userName);
    void setNickName(std::string nickName);
    void setIP(std::string IP);
    void setMessage(Message msg);
    void setPass(bool newPass);
    void setnbrChannels(char sign);

    
    
    // utils
    void disconnect();
    void sendMsg(std::string str);
    
    void consume_message(const std::string& s) {
        this->_msg.consume_buffer(s);
    }
};

#endif
