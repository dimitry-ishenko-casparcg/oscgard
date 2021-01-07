////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "actions.hpp"
#include "args.hpp"
#include "server.hpp"
#include "system.hpp"

#include <asio.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace asio::ip;
namespace fs = std::filesystem;

////////////////////////////////////////////////////////////////////////////////
#if !defined(VERSION)
#  define VERSION 0
#endif

#define TEXT(x) TEXT_(x)
#define TEXT_(x) #x

////////////////////////////////////////////////////////////////////////////////
std::string usage(const std::string& name)
{
    return "Usage: " + name + R"( [option] [path]

Where [path] is an optional path to an alternative settings file with OSC
address patterns and their corresponding commands.

Option is one or more of the following:

    --address=<addr>    Specify IP address to bind to. Default: bind to all
                        addresses.

    --help, -h          Print this help screen and exit.

    --port=<n>          Specify port to listen on. Default: 6260.

    --version, -v       Show version and exit.)";
}

////////////////////////////////////////////////////////////////////////////////
std::string version(const std::string& name)
{
    return name + " version " TEXT(VERSION);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int exit_code = 0;
    try
    {
        auto name = fs::path(argv[0]).filename();

        auto args = src::args::read_from(argc, argv);
        if(args.help)
        {
            std::cout << usage(name) << std::endl;
        }
        else if(args.version)
        {
            std::cout << version(name) << std::endl;
        }
        else
        {
            if(args.path.empty())
            {
                args.path = src::data_path() / name / "actions.conf";
                fs::create_directory(args.path.parent_path());

                if(!fs::exists(args.path)) std::fstream{ args.path, std::ios::out };
            }

            std::cout << "Reading actions from " << args.path << std::endl;
            auto acts = src::actions::read_from(args.path);

            if(args.port == 0) args.port = 6260;

            auto local = args.address.is_unspecified()
                ? udp::endpoint(udp::v4(), args.port)
                : udp::endpoint(args.address, args.port);
            std::cout << "Listening on " << local << std::endl;

            asio::io_context io;
            src::server server(io, local, acts);

            std::cout << "Starting event loop" << std::endl;
            io.run();
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit_code = 1;
    }
    catch(...)
    {
        std::cerr << "???" << std::endl;
        exit_code = 1;
    }
    return exit_code;
}
