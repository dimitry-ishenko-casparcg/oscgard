////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "action.hpp"
#include "actions.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
namespace
{

auto get_string(std::stringstream& ss)
{
    std::string s;
    ss >> s >> std::ws;
    return s;
}

bool get_equal_sign(std::stringstream& ss)
{
    char ch { };
    ss >> ch >> std::ws;
    return ch == '=';
}

auto get_quoted(std::stringstream& ss)
{
    std::string s;
    ss >> std::quoted(s) >> std::ws;
    return s;
}

}

////////////////////////////////////////////////////////////////////////////////
actions read_actions(const fs::path& file)
{
    std::fstream fs{ file, std::ios::in };
    if(!fs.good()) throw std::invalid_argument{ "Can't open file" };

    actions acts;

    std::string read;
    for(int n = 1; std::getline(fs, read); ++n)
    {
        std::stringstream ss{ read };
        ss >> std::ws;

        auto name = get_string(ss);
        if(name.empty() || name[0] == '#') continue;

        if(!get_equal_sign(ss)) throw invalid_line{ n, "Missing '=' sign" };

        fs::path file{ get_quoted(ss) };
        if(file.empty()) throw invalid_line{ n, "Missing or empty file name" };

        std::vector<std::string> args;
        while(!ss.eof()) args.push_back(get_quoted(ss));

        acts.emplace(name, action{ std::move(file), std::move(args) });
    }

    return acts;
}

////////////////////////////////////////////////////////////////////////////////
}
