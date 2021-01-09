////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "action.hpp"
#include "system.hpp"

#include <iomanip>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
namespace
{

std::ostream& operator<<(std::ostream& os, const action& act)
{
    os << act.file << " {";
    for(auto const& arg : act.args) os << " " << std::quoted(arg);
    return os << " }";
}

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

void replace_token(std::string& s, const std::string& tkn, const std::string& to)
{
    std::size_t p = 0;
    while((p = s.find(tkn, p)) != std::string::npos)
    {
        s.replace(p, tkn.size(), to);
        p += to.size();
    }
}

auto replace_tokens(const std::vector<std::string>& args, const osc::message& msg)
{
    auto args_ { args };

    for(auto& arg : args_) replace_token(arg, "{A}", msg.address());

    int n = 0;
    for(auto const& val : msg.values())
    {
        auto tkn = '{' + std::to_string(n++) + '}';
        auto to = to_string(val);
        for(auto& arg : args_) replace_token(arg, tkn, to);
    }

    return args_;
}

}

////////////////////////////////////////////////////////////////////////////////
void action::operator()(const osc::message& msg)
{
    auto args_ = replace_tokens(args, msg);

    if(auto pid = fork(); pid == child)
        exec(file, args_);
    else std::cout << "Starting " << (*this) << " as process " << pid << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
}
