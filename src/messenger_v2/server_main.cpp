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

#include <iostream>
#include <string>

#include "server/server.h"

int main()
{
    wstux::server::server s(14088);
    if (! s.listen()) {
        std::cout << "Failed to listen to" << std::endl;
        return 1;
    }

    wstux::server::transport::ptr p_tr = s.accept();
    while (true) {
        std::string msg;

        std::cout << "[server]: ";
        std::cin >> msg;
        p_tr->send(msg);

        msg = p_tr->receive<std::string>();
        std::cout << "[client]: " << msg << std::endl;
    }

    return 0;
}

