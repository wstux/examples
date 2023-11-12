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

#include "server/client.h"

int main()
{
    wstux::server::client c("127.0.0.1", 14088);
    if (! c.connect()) {
        std::cout << "Failed to listen to" << std::endl;
        return 1;
    }
    
    while (true) {
        std::string msg;

        msg = c.receive<std::string>();
        std::cout << "[server]: " << msg << std::endl;

        std::cout << "[client]: ";
        std::cin >> msg;
        c.send(msg);
    }

    return 0;
}

