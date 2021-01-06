////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "server.hpp"
#include "settings.hpp"

#include <asio.hpp>
#include <exception>
#include <iostream>

using namespace asio::ip;

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int exit_code = 0;
    try
    {
        asio::io_context io;

        auto conf = src::settings::read(
            (argc > 1) ? argv[1] : src::settings::default_path(argv[0])
        );
        src::server server(io, std::move(conf));

        std::cout << "Starting event loop" << std::endl;
        io.run();
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
