////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "system.hpp"

#include <cerrno>
#include <system_error>
#include <vector>

#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
fork_t fork()
{
    auto pid = ::fork();
    if(pid < 0) throw std::system_error(errno, std::generic_category());

    return pid ? parent : child;
}

////////////////////////////////////////////////////////////////////////////////
void exec(const path& file, const src::args& args)
{
    std::vector<const char*> argv;

    argv.push_back(file.c_str());
    for(auto const& arg : args) argv.push_back(arg.c_str());
    argv.push_back(nullptr);

    execvp(argv[0], const_cast<char* const*>(argv.data()));

    throw std::system_error(errno, std::generic_category());
}
    
////////////////////////////////////////////////////////////////////////////////
}
