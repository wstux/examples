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

#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>

#include "server/server.h"

namespace wstux {
namespace server {

server::server(const port_t port)
    : m_port(port)
{}

server::~server()
{
    close();
}

transport::ptr server::accept()
{
    if (! is_running()) {
        return transport::ptr();
    }

    struct ::sockaddr_in client;
    unsigned int len = sizeof(client);
    int conn_fd = ::accept(m_sock_fd, (struct ::sockaddr*)&client, &len);

    return std::make_shared<transport>(conn_fd);
}

void server::close()
{
    if (is_running()) {
        ::close(m_sock_fd);
    }
    m_sock_fd = -1;
}

bool server::listen()
{
    if (is_running()) {
        return true;
    }

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
        close();
        return false;
    }

    // Server is ready to listen and verification 
    rc = ::listen(m_sock_fd, 5);
    if (rc != 0) {
        close();
    }
    return (rc == 0);
}

} // namespace server
} // namespace wstux

