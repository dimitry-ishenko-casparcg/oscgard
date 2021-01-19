////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "actions.hpp"
#include "pgm/args.hpp"
#include "server.hpp"
#include "system.hpp"

#include <asio.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////////////////////////
#if !defined(VERSION)
#  define VERSION 0
#endif

#define TEXT(x) TEXT_(x)
#define TEXT_(x) #x

////////////////////////////////////////////////////////////////////////////////
std::string version(const std::string& name)
{
    return name + " version " TEXT(VERSION);
}

////////////////////////////////////////////////////////////////////////////////
auto to_address(const std::string& s)
{
    asio::error_code ec;
    auto address = asio::ip::make_address(s, ec);

    if(!ec) return address;
    else throw pgm::invalid_argument{ "Invalid IP address", s };
}

////////////////////////////////////////////////////////////////////////////////
auto to_port(const std::string& s)
{
    char* end;
    auto ul = std::strtoul(s.data(), &end, 0);

    if(ul <= UINT16_MAX && end == (s.data() + s.size()))
        return static_cast<std::uint16_t>(ul);
    else throw pgm::invalid_argument{ "Invalid port number", s };
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int exit_code = 0;
    try
    {
        auto name{ fs::path(argv[0]).filename() };
        pgm::args args
        {
            { "-a", "--address", "addr", "Specify IP address to bind to. Default: 0.0.0.0\n"
                                         "(bind to all addresses)."             },
            { "-p", "--port", "N",       "Specify port number to listen on Default: 6260." },
            { "-h", "--help",            "Print this help screen and exit."     },
            { "-v", "--version",         "Show version number and exit."        },
            { "actions?",                "Path to alternate actions file."      }
        };
        args.parse(argc, argv);

        if(args["--help"])
            std::cout << args.usage(name) << std::endl;

        else if(args["--version"])
            std::cout << version(name) << std::endl;

        else
        {
            fs::path path{ args["actions"].value_or("") };
            if(path.empty())
            {
                path = src::data_path() / name / "actions.conf";

                fs::create_directory(path.parent_path());
                if(!fs::exists(path)) std::fstream{ path, std::ios::out };
            }

            std::cout << "Reading actions from " << path << "." << std::endl;
            auto acts{ src::actions::read_from(path) };

            auto address{ to_address(args["--address"].value_or("0.0.0.0")) };
            auto port{ to_port(args["--port"].value_or("6260")) };

            asio::ip::udp::endpoint local{ address, port };
            std::cout << "Listening on " << local << "." << std::endl;

            asio::io_context io;
            src::server server(io, local, acts);

            src::set_interrupt_callback([&](int signal)
            {
                std::cout << "Received signal " << signal << " - exiting." << std::endl;
                io.stop();
            });

            std::cout << "Starting event loop." << std::endl;
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
