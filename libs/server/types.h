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

#ifndef _EXAMPLES_SERVER_TYPES_H_
#define _EXAMPLES_SERVER_TYPES_H_

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace wstux {
namespace server {

using address_t = std::string;
using port_t = int;
using server_info_t = std::pair<address_t, port_t>;

using servers_list_t = std::unordered_map<std::string, server_info_t>;


} // namespace server
} // namespace wstux

#endif /* _EXAMPLES_SERVER_TYPES_H_ */

