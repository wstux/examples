/*
 * experiments
 * Copyright (C) 2023  Chistyakov Alexander
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <netinet/in.h> 
#include <sys/socket.h>

#include <strings.h>

#include <iostream>
#include <string>

namespace {

class server
{
public:
    explicit server(const int port)
        : m_port(port)
        , m_sock_fd(-1)
        , m_conn_fd(-1)
    {}

    ~server()
    {
        if (m_sock_fd != -1) {
            ::close(m_sock_fd);
        }
        if (m_conn_fd != -1) {
            ::close(m_conn_fd);
        }
    }

    bool listen()
    {
        // create socket 
        m_sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (m_sock_fd == -1) {
            return false;
        }

        struct ::sockaddr_in serv_addr;
        ::bzero(&serv_addr, sizeof(serv_addr)); 
   
        // assign IP, PORT 
        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_addr.s_addr = ::htonl(INADDR_ANY); 
        serv_addr.sin_port = ::htons(m_port); 

        // Binding newly created socket to given IP and verification
        int rc = ::bind(m_sock_fd, (struct ::sockaddr*)&serv_addr, sizeof(serv_addr)); 
        if (rc != 0) {
            return false;
        }

        // Server is ready to listen and verification 
        rc = ::listen(m_sock_fd, 5);
        if (rc != 0) {
            return false;
        }

        struct ::sockaddr_in client;
        unsigned int len = sizeof(client);
        m_conn_fd = ::accept(m_sock_fd, (struct ::sockaddr*)&client, &len);
        if (m_conn_fd < 0) {
            return false;
        }

        return true;
    }

    std::string read()
    {
        constexpr size_t kMaxMsgSize = 140;

        char buffer[kMaxMsgSize];
        ::bzero(buffer, kMaxMsgSize);

        // read the message from client and copy it in buffer
        ::read(m_conn_fd, buffer, kMaxMsgSize);
        return std::string(buffer);
    }

    bool send(const std::string& msg)
    {
        constexpr size_t kMaxMsgSize = 140;
        if (msg.size() > kMaxMsgSize) {
            return false;
        }

        int rc = ::send(m_conn_fd, msg.c_str(), msg.size(), 0);
        return (rc > 0);
    }

private:
    const int m_port;
    int m_sock_fd;
    int m_conn_fd;
};

} // <anonymous> namespace

int main()
{
    server s(14088);
    if (! s.listen()) {
        std::cout << "Failed to listen to" << std::endl;
        return 1;
    }

    while (true) {
        std::string msg;

        std::cout << "[server]: ";
        std::cin >> msg;
        s.send(msg);

        msg = s.read();
        std::cout << "[client]: " << msg << std::endl;
    }

    return 0;
}

