#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>

int main() {
    // Create and bind a server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "socket() failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); // Example port number

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "bind() failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) < 0) {
        std::cerr << "listen() failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Vector to store client sockets
    std::vector<pollfd> fds;
    pollfd tmp;
    tmp.fd = serverSocket;
    tmp.events = POLLIN;
    fds.push_back(tmp);

    // Main server loop
    while (true) {
        int numReady = poll(&fds[0], fds.size(), -1);
        if (numReady < 0) {
            std::cerr << "poll() failed" << std::endl;
            break;
        }

        // Check for new connections
        if (fds[0].revents & POLLIN) {
            int clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket < 0) {
                std::cerr << "accept() failed" << std::endl;
            } else {
                std::cout << "New client connected" << std::endl;
                pollfd tmp;
                tmp.fd = clientSocket;
                tmp.events = POLLIN;
                fds.push_back(tmp);
            }
        }

        // Check for data on client sockets
        for (size_t i = 1; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                char buffer[1024];
                int bytesReceived = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) {
                    // Error or connection closed by client
                    if (bytesReceived == 0) {
                        // Connection closed
                        std::cout << "Client disconnected" << std::endl;
                    } else {
                        // Error
                        std::cerr << "recv() failed" << std::endl;
                    }
                    // Remove the client socket from the vector
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i; // Adjust index after erasing element
                } else {
                    // Process the received data (e.g., handle IRC messages)
                    // Here you can parse and handle the incoming message from the client
                    // For example, you might parse IRC commands and respond accordingly
                    // HandleIRCMessage(buffer);
                    buffer[bytesReceived] = 0;
                    std::cout << "Received message from client " << fds[i].fd << ": " << buffer << std::endl;
                }
            }
        }
    }

    // Close all client sockets
    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
