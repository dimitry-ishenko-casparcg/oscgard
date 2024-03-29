////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020-2021 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "util.hpp"

#include <cerrno>
#include <csignal>
#include <cstdlib> // std::getenv
#include <system_error>
#include <vector>

#include <sys/wait.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
pid_t fork()
{
    auto pid = ::fork();
    if(pid < 0) throw std::system_error{ errno, std::generic_category() };

    return pid;
}

////////////////////////////////////////////////////////////////////////////////
void exec(const fs::path& file, const std::vector<std::string>& args)
{
    std::vector<const char*> argv;

    argv.push_back(file.c_str());
    for(auto const& arg : args) argv.push_back(arg.c_str());
    argv.push_back(nullptr);

    execvp(argv[0], const_cast<char* const*>(argv.data()));

    throw std::system_error{ errno, std::generic_category() };
}

////////////////////////////////////////////////////////////////////////////////
void on_child_exit(child_exit_callback cb)
{
    static child_exit_callback cb_;

    if(!cb_) std::signal(SIGCHLD, [](int)
    {
        pid_t pid;
        int status;

        while((pid = waitpid(-1, &status, WNOHANG)) > 0) cb_(pid, status);
    });

    cb_ = std::move(cb);
    if(!cb_) std::signal(SIGCHLD, SIG_DFL);
}

////////////////////////////////////////////////////////////////////////////////
void on_interrupt(interrupt_callback cb)
{
    static interrupt_callback cb_;

    if(!cb_)
    {
        std::signal(SIGINT, [](int signal) { cb_(signal); });
        std::signal(SIGTERM, [](int signal) { cb_(signal); });
    }

    cb_ = std::move(cb);

    if(!cb_)
    {
        std::signal(SIGINT, SIG_DFL);
        std::signal(SIGTERM, SIG_DFL);
    }
}

////////////////////////////////////////////////////////////////////////////////
fs::path xdg_config_home()
{
    auto val = std::getenv("XDG_CONFIG_HOME");
    if(val) return fs::path{ val };

    val = std::getenv("HOME");
    if(val) return fs::path{ val } / ".config";

    return { };
}

////////////////////////////////////////////////////////////////////////////////
bool is_root() { return geteuid() == 0; }

////////////////////////////////////////////////////////////////////////////////
}
