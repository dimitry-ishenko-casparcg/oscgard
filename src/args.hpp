////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef SRC_ARGS_HPP
#define SRC_ARGS_HPP

////////////////////////////////////////////////////////////////////////////////
#include "system.hpp"

#include <asio.hpp>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
struct args
{
    asio::ip::address address;
    std::uint16_t port = 0;

    bool version = false;
    bool help = false;

    fs::path path;

    static args read_from(int argc, char* argv[]);
};

////////////////////////////////////////////////////////////////////////////////
struct duplicate_option : std::invalid_argument
{
    explicit duplicate_option(const std::string& opt) :
        std::invalid_argument("Duplication option: " + opt)
    { }
};

struct invalid_option : std::invalid_argument
{
    explicit invalid_option(const std::string& opt) :
        std::invalid_argument("Invalid option: " + opt)
    { }
};

struct missing_value : std::invalid_argument
{
    explicit missing_value(const std::string& opt) :
        std::invalid_argument("Missing option value: " + opt)
    { }
};

struct invalid_value : std::invalid_argument
{
    explicit invalid_value(const std::string& opt) :
        std::invalid_argument("Invalid option value: " + opt)
    { }
};

struct extra_param : std::invalid_argument
{
    explicit extra_param(const std::string& param) :
        std::invalid_argument("Extraneous parameter: " + param)
    { }
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
