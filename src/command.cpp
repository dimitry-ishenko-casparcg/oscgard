////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "command.hpp"

#include <cerrno>
#include <csignal>
#include <iostream>
#include <mutex> // std::call_once
#include <system_error>

#include <sys/wait.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
command::command(path file, src::args args) :
    file_(std::move(file)), args_(std::move(args))
{
    static std::once_flag install;
    std::call_once(install, std::signal, SIGCHLD, &command::reap);
}

////////////////////////////////////////////////////////////////////////////////
void command::operator()(const osc::message& m)
{
    src::args args = args_;
    //

    spawn(file_, args);
}

////////////////////////////////////////////////////////////////////////////////
void command::spawn(const path& file, const src::args& args)
{
    if(auto pid = fork(); pid > 0)
    {
        std::cout << "Starting " << file << " " << args << " as process " << pid << std::endl;
    }
    else
    {
        if(pid == 0) // child
        {
            std::vector<const char*> argv;

            argv.push_back(file.c_str());
            for(auto const& arg : args) argv.push_back(arg.c_str());
            argv.push_back(nullptr);

            execvp(argv[0], const_cast<char* const*>(argv.data()));
        }

        throw std::system_error(errno, std::generic_category());
    }
}

////////////////////////////////////////////////////////////////////////////////
void command::reap(int)
{
    pid_t pid;
    int status;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        std::cout << "Process " << pid << " exited with status " << status << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
}
