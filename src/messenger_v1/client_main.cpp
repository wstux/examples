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
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <sys/socket.h>

#include <strings.h>

#include <iostream>
#include <string>

namespace {

class client
{
public:
    client(const std::string& addr, const int port)
        : m_addr(addr)
        , m_port(port)
        , m_sock_fd(-1)
    {}

    ~client()
    {
        if (m_sock_fd != -1) {
            ::close(m_sock_fd);
        }
    }

    bool connect()
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
        serv_addr.sin_addr.s_addr = ::inet_addr(m_addr.c_str()); 
        serv_addr.sin_port = ::htons(m_port); 

        // Connect the client to server
        int rc = ::connect(m_sock_fd, (struct ::sockaddr*)&serv_addr, sizeof(serv_addr)); 
        return (rc == 0);
    }

    std::string read()
    {
        constexpr size_t kMaxMsgSize = 140;

        char buffer[kMaxMsgSize];
        ::bzero(buffer, kMaxMsgSize);

        // read the message from client and copy it in buffer
        ::read(m_sock_fd, buffer, kMaxMsgSize);
        return std::string(buffer);
    }

    bool send(const std::string& msg)
    {
        constexpr size_t kMaxMsgSize = 140;
        if (msg.size() > kMaxMsgSize) {
            return false;
        }

        int rc = ::send(m_sock_fd, msg.c_str(), msg.size(), 0);
        return (rc > 0);
    }

private:
    const std::string m_addr;
    const int m_port;
    int m_sock_fd;
};

} // <anonymous> namespace

int main()
{
    client c("127.0.0.1", 14088);
    if (! c.connect()) {
        std::cout << "Failed to listen to" << std::endl;
        return 1;
    }
    
    while (true) {
        std::string msg;

        msg = c.read();
        std::cout << "[server]: " << msg << std::endl;

        std::cout << "[client]: ";
        std::cin >> msg;
        c.send(msg);
    }

    return 0;
}

