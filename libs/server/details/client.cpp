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
#include <arpa/inet.h>
#include <netinet/in.h>

#include "server/client.h"

namespace wstux {
namespace server {

client::client(const address_t& addr, const port_t port)
    : m_addr(addr)
    , m_port(port)
    , m_sock_fd(-1)
{}

client::~client()
{
    close();
}

void client::close()
{
    if (is_connected()) {
        ::close(m_sock_fd);
    }
    m_sock_fd = -1;
}

bool client::connect()
{
    if (is_connected()) {
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
    serv_addr.sin_addr.s_addr = ::inet_addr(m_addr.c_str()); 
    serv_addr.sin_port = ::htons(m_port);

    // Connect the client to server
    int rc = ::connect(m_sock_fd, (struct ::sockaddr*)&serv_addr, sizeof(serv_addr));
    if (rc != 0) {
        close();
    }
    return (rc == 0);
}

} // namespace server
} // namespace wstux

