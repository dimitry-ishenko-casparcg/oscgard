////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "command.hpp"
#include "settings.hpp"

#include <asio.hpp>
#include <cstdlib> // std::getenv
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
path settings::default_path(const char* argv_0)
{
#if defined(_WIN32)
    auto file = path(std::getenv("APPDATA"));
#elif defined(__APPLE__)
    auto file = path(std::getenv("HOME")) / "Library" / "Application Support";
#elif defined(__unix__)
    auto file = path(std::getenv("HOME")) / ".local" / "share";
#else
    #error "Unsupported platform"
#endif

    file = file / path(argv_0).stem() / "settings.conf";
    return std::filesystem::weakly_canonical(file);
}

////////////////////////////////////////////////////////////////////////////////
namespace
{

auto parse_name(std::stringstream& ss)
{
    std::string s;
    ss >> s >> std::ws;
    return s;
}

bool parse_equal(std::stringstream& ss)
{
    char ch { };
    ss >> ch >> std::ws;
    return ch == '=';
}

auto parse_address(std::stringstream& ss)
{
    std::string s = parse_name(ss);
    asio::error_code ec;
    return asio::ip::make_address(s, ec);
}

auto parse_port(std::stringstream& ss)
{
    port p = 0;
    ss >> p >> std::ws;
    return p;
}

auto parse_quoted(std::stringstream& ss)
{
    std::string s;
    ss >> std::quoted(s) >> std::ws;
    return s;
}

}

////////////////////////////////////////////////////////////////////////////////
settings settings::read(const src::path& file)
{
    std::cout << "Reading settings from " << file << std::endl;

    std::fstream fs(file, std::ios::in);
    if(!fs.is_open()) throw std::invalid_argument("Can't open file");

    settings conf;

    std::string line;
    for(int n = 1; std::getline(fs, line); ++n)
    {
        std::stringstream ss(line);
        ss >> std::ws;

        auto name = parse_name(ss);
        if(name.empty() || name[0] == '#') continue;

        if(!parse_equal(ss)) throw invalid_line(n, "Missing '=' sign");

        if(name == "address")
        {
            conf.address_ = parse_address(ss);
            if(conf.address_.is_unspecified() || !ss.eof())
                throw invalid_line(n, "Invalid IP address");
        }
        else if(name == "port")
        {
            conf.port_ = parse_port(ss);
            if(conf.port_ == 0 || !ss.eof()) throw invalid_line(n, "Invalid port #");
        }
        else
        {
            path file = parse_quoted(ss);
            if(file.empty()) throw invalid_line(n, "Missing or empty file name");

            src::args args;
            while(!ss.eof()) args.push_back(parse_quoted(ss));

            conf.space_.emplace_back(std::move(name),
                command { std::move(file), std::move(args) }
            );
        }
    }

    return conf;
}

////////////////////////////////////////////////////////////////////////////////
}
