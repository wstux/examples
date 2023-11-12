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

#ifndef _EXPERIMENTS_SERVER_TRANSPORT_H_
#define _EXPERIMENTS_SERVER_TRANSPORT_H_

#include <unistd.h>

#include <memory>

#include "server/details/utils.h"

namespace wstux {
namespace server {

class transport final
{
public:
    using ptr = std::shared_ptr<transport>;

    explicit transport(const int fd)
        : m_fd(fd)
    {}

    ~transport()
    {
        if (m_fd != -1) {
            ::close(m_fd);
        }
    }

    bool is_open() const { return (m_fd != -1); }

    int receive(char* p_buffer, const size_t buf_size) const
    {
        if (! is_open()) {
            return 0;
        }
        return details::utils::receive(m_fd, p_buffer, buf_size);
    }

    template<typename TContainer, size_t kChunkSize = 140>
    TContainer receive() const
    {
        if (! is_open()) {
            return TContainer();
        }
        return details::utils::receive<TContainer, kChunkSize>(m_fd);
    }

    int send(char* p_buffer, const size_t buf_size) const
    {
        if (! is_open()) {
            return 0;
        }
        return details::utils::receive(m_fd, p_buffer, buf_size);
    }

    template<typename TContainer, size_t kChunkSize = 140>
    bool send(const TContainer& msg) const
    {
        if (! is_open()) {
            return false;
        }
        return details::utils::send<TContainer, kChunkSize>(m_fd, msg);
    }

private:
    int m_fd;
};

} // namespace server
} // namespace wstux

#endif /* _EXPERIMENTS_SERVER_TRANSPORT_H_ */

