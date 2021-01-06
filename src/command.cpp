////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "command.hpp"
#include "system.hpp"

#include <iomanip>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
namespace std
{

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& args)
{
    os << "{";
    for(auto const& arg : args) os << " " << std::quoted(arg);
    return os << " }";
}

}

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
void command::operator()(const osc::message& m)
{
    auto args_ { args };

    //

    if(auto pid = fork(); pid == child)
        exec(file, args_);
    else std::cout << "Starting " << file << " " << args_ << " as process " << pid << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
}
