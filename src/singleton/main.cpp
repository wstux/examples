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

#include <functional>
#include <iostream>

namespace {

template <typename T>
class singleton
{
public:
    template <typename... Args>
    static T& get_instance(Args&&... args) 
    {
        static T instance(std::forward<Args>(args)...);
        return instance;
    }

protected:
    singleton() {}
    virtual ~singleton() {}

private:
    singleton(const singleton&) {}
    singleton(singleton&&) {}

    singleton& operator=(const singleton&) {}
    singleton& operator=(singleton&&) {}
};

class single_class final : public singleton<single_class> {};

} // <anonymous> namespace

int main()
{
    single_class& instance1 = single_class::get_instance();
    single_class& instance2 = single_class::get_instance();
    single_class& instance3 = single_class::get_instance();

    std::cout << "&instance1 = " << &instance1 << std::endl;
    std::cout << "&instance2 = " << &instance2 << std::endl;
    std::cout << "&instance3 = " << &instance3 << std::endl;

    return 0;
}

