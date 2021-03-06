////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "action.hpp"
#include "util.hpp"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
namespace
{

auto to_string(const osc::value& v)
{
    std::string s;

         if(v.is_int32 ()) s = std::to_string(v.to_int32 ());
    else if(v.is_float ()) s = std::to_string(v.to_float ());
    else if(v.is_string()) s = v.to_string();
    else if(v.is_blob  ()) s = ""; // TODO: convert to hex
    else if(v.is_int64 ()) s = std::to_string(v.to_int64 ());
    else if(v.is_time  ()) s = '@' + std::to_string(v.to_time().time_since_epoch().count());
    else if(v.is_double()) s = std::to_string(v.to_double());
    else if(v.is_char  ()) s = v.to_char();
    else if(v.is_true  ()) s = "true";
    else if(v.is_false ()) s = "false";
    else if(v.is_nil   ()) s = "nil";
    else if(v.is_inf   ()) s = "inf";

    return s;
}

void replace_token(std::string& s, const std::string& from, const std::string& to)
{
    std::size_t p = 0;
    while((p = s.find(from, p)) != std::string::npos)
    {
        s.replace(p, from.size(), to);
        p += to.size();
    }
}

void replace_tokens(std::vector<std::string>& args, const osc::message& message)
{
    for(auto& arg : args) replace_token(arg, "{A}", message.address());

    int n = 0;
    for(auto const& value : message.values())
    {
        auto from = '{' + std::to_string(n++) + '}';
        auto to = to_string(value);
        for(auto& arg : args) replace_token(arg, from, to);
    }
}

}

////////////////////////////////////////////////////////////////////////////////
void action::operator()(const osc::message& messsage)
{
    if(auto pid = fork(); pid == 0) // child
    {
        replace_tokens(args, messsage);
        exec(file, args);
    }
    else std::cout << "Started " << file << " as process " << pid << "." << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
}
