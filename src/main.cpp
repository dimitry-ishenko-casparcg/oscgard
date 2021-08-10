////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "actions.hpp"
#include "pgm/args.hpp"
#include "server.hpp"
#include "util.hpp"

#include <asio.hpp>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

#if !defined(VERSION)
#  define VERSION "0"
#endif

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
try
{
    auto name = fs::path{ argv[0] }.filename();

    std::string def_address = "127.0.0.1";
    std::string def_port = "6260";

    auto def_actions = name / "actions.conf";
    auto def_root_actions = "/etc" / def_actions;
    auto def_user_actions = src::xdg_config_home() / def_actions;

    pgm::args args
    {{
        { "-a", "--address", "addr", "Specify IP address to bind to. Default: " + def_address + "." },
        { "-p", "--port", "N",       "Specify port number to listen on. Default: " + def_port + "." },
        { "-h", "--help",            "Print this help screen and exit."                             },
        { "-v", "--version",         "Show version number and exit."                                },

        { "actions?",                "Specify path to the actions file.\n\n"
                                     "The default path is " + def_root_actions.string() + ",\n"
                                     "when running as root;\n"
                                     "and $XDG_CONFIG_HOME/" + def_actions.string() + " otherwise." },
    }};

    // delay exception handling to process --help and --version
    std::exception_ptr ep;
    try { args.parse(argc, argv); }
    catch(...) { ep = std::current_exception(); }

    if(args["--help"])
    {
        std::cout << "\n" << args.usage(name) << "\n" << std::endl;
    }
    else if(args["--version"])
    {
        std::cout << name.string() << " version " << VERSION << std::endl;
    }
    else if(ep)
    {
        std::rethrow_exception(ep);
    }
    else
    {
        auto val = args["actions"].value_or("");
        auto path = val.size() ? fs::path{ val } : src::is_root() ? def_root_actions : def_user_actions;

        std::cout << "Reading actions from " << path << "." << std::endl;
        auto actions = src::read_actions(path);

        asio::ip::udp::endpoint local{
            to_address(args["--address"].value_or(def_address)),
            to_port(args["--port"].value_or(def_port))
        };
        std::cout << "Listening on " << local << "." << std::endl;

        asio::io_context io;
        src::server server{ io, local, actions };

        src::on_interrupt([&](int signal)
        {
            std::cout << "Received signal " << signal << " - exiting." << std::endl;
            io.stop();
        });

        std::cout << "Starting event loop." << std::endl;
        io.run();
    }

    return 0;
}
catch(std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
catch(...)
{
    std::cerr << "???" << std::endl;
    return 1;
}
