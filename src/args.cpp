////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "args.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
namespace
{

bool starts_with(const std::string& s, const std::string& prefix)
{
    return s.compare(0, prefix.size(), prefix) == 0;
}

auto to_address(const std::string& s)
{
    asio::error_code ec;
    return asio::ip::make_address(s, ec);
}

auto to_port(const std::string& s)
{
    char* end;
    auto ul = std::strtoul(s.data(), &end, 0);

    std::uint16_t p = 0;
    if(ul <= UINT16_MAX && end == (s.data() + s.size())) p = ul;

    return p;
}

}

////////////////////////////////////////////////////////////////////////////////
args args::read_from(int argc, char* argv[])
{
    src::args args;

    for(int n = 1; n < argc; ++n)
    {
        std::string arg = argv[n];
        if(arg.empty()) continue;

        if(arg == "--address") throw missing_value(arg);
        else if(starts_with(arg, "--address="))
        {
            if(args.address.is_unspecified())
            {
                args.address = to_address( arg.substr(sizeof("--address=")-1) );
                if(args.address.is_unspecified()) throw invalid_value(arg);
            }
            else throw duplicate_option(arg);
        }
        else if(arg == "-h" || arg == "--help")
        {
            args.help = true;
            return args;
        }
        else if(arg == "--port") throw missing_value(arg);
        else if(starts_with(arg, "--port="))
        {
            if(args.port == 0)
            {
                args.port = to_port( arg.substr(sizeof("--port=")-1) );
                if(args.port == 0) throw invalid_value(arg);
            }
            else throw duplicate_option(arg);
        }
        else if(arg == "-v" || arg == "--version")
        {
            args.version = true;
            return args;
        }
        else if(arg[0] == '-') throw invalid_option(arg);
        else
        {
            if(args.path.empty()) args.path = arg;
            else throw extra_param(arg);
        }
    }

    return args;
}

////////////////////////////////////////////////////////////////////////////////
}

