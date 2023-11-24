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

#ifndef _EXAMPLES_SERVER_SERVER_H_
#define _EXAMPLES_SERVER_SERVER_H_

#include "server/transport.h"
#include "server/types.h"

namespace wstux {
namespace server {

class server
{
public:
    explicit server(const port_t port);
    virtual ~server();

    transport::ptr accept();

    void close();

    bool is_running() const { return (m_sock_fd != -1); }

    bool listen();

private:
    const port_t m_port;
    int m_sock_fd = -1;
};

} // namespace server
} // namespace wstux

#endif /* _EXAMPLES_SERVER_SERVER_H_ */

