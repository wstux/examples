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

#ifndef _EXAMPLES_SERVER_UTILS_H_
#define _EXAMPLES_SERVER_UTILS_H_

#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>

namespace wstux {
namespace server {
namespace details {

struct utils
{
    static int receive(int sock_fd, char* p_buffer, size_t buf_size)
    {
        ::bzero(p_buffer, buf_size);
        // read the message from client and copy it in buffer
        return ::read(sock_fd, p_buffer, buf_size);
    }

    template<typename TContainer, size_t kChunkSize = 140>
    static TContainer receive(int sock_fd)
    {
        TContainer msg;

        char chunk[kChunkSize];
        int rc = receive(sock_fd, chunk, kChunkSize);
        msg.insert(msg.end(), chunk, chunk + rc);

        return msg;
    }

    static int send(int sock_fd, const char* p_buffer, size_t buf_size)
    {
        return ::send(sock_fd, p_buffer, buf_size, 0);
    }

    template<typename TContainer, size_t kChunkSize = 140>
    static bool send(int sock_fd, const TContainer& msg)
    {
        int rc = send(sock_fd, static_cast<const char*>(msg.data()), msg.size());
        return ((rc > 0) && ((size_t)rc == msg.size()));
    }
};

} // namespace details
} // namespace server
} // namespace wstux

#endif /* _EXAMPLES_SERVER_UTILS_H_ */

