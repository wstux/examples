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

#ifndef _EXAMPLES_SERVER_CLIENT_H_
#define _EXAMPLES_SERVER_CLIENT_H_

#include "server/transport.h"
#include "server/types.h"
#include "server/details/utils.h"

namespace wstux {
namespace server {

class client
{
public:
    client(const address_t& addr, const port_t port);
    virtual ~client();

    void close();

    bool connect();

    bool is_connected() const { return (m_sock_fd != -1); }

    int receive(char* p_buffer, const size_t buf_size) const
    {
        if (! is_connected()) {
            return 0;
        }
        return details::utils::receive(m_sock_fd, p_buffer, buf_size);
    }

    template<typename TContainer, size_t kChunkSize = 140>
    TContainer receive() const
    {
        if (! is_connected()) {
            return TContainer();
        }
        return details::utils::receive<TContainer, kChunkSize>(m_sock_fd);
    }

    int send(char* p_buffer, const size_t buf_size) const
    {
        if (! is_connected()) {
            return 0;
        }
        return details::utils::receive(m_sock_fd, p_buffer, buf_size);
    }

    template<typename TContainer, size_t kChunkSize = 140>
    bool send(const TContainer& msg) const
    {
        if (! is_connected()) {
            return false;
        }
        return details::utils::send<TContainer, kChunkSize>(m_sock_fd, msg);
    }

private:
    const address_t m_addr;
    const port_t m_port;
    int m_sock_fd;
};

} // namespace server
} // namespace wstux

#endif /* _EXAMPLES_SERVER_CLIENT_H_ */

