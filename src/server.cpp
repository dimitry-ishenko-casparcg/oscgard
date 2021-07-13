////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "server.hpp"
#include "util.hpp"

#include <chrono>
#include <exception>
#include <functional> // std::bind
#include <iostream>

using namespace std::chrono_literals;
using namespace std::placeholders;

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
server::server(asio::io_context& io, const udp::endpoint& local, const src::actions& actions) :
    socket_{ io }, space_{ std::bind(&server::callback_sched, this, _1, _2) }
{
    on_child_exit([](pid_t pid, int status)
    {
        std::cout << "Process " << pid << " exited with status " << status << "." << std::endl;
    });

    space_.add("/cancel", std::bind(&server::cancel, this, _1));
    for(auto const& [ cmd, action ] : actions) space_.add(cmd, action);

    socket_.open(udp::v4());
    socket_.bind(local);

    async_recv();
}

////////////////////////////////////////////////////////////////////////////////
void server::async_recv()
{
    socket_.async_wait(udp::socket::wait_read, [=](const asio::error_code& ec)
    {
        if(!ec)
        {
            osc::packet packet{ static_cast<osc::int32>(socket_.available()) };
            udp::endpoint remote;

            socket_.receive_from(asio::buffer(packet.data(), packet.size()), remote);
            try
            {
                auto element = packet.parse();
                space_.dispatch(element);
            }
            catch(std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }

            async_recv();
        }
    });
}

////////////////////////////////////////////////////////////////////////////////
void server::callback_sched(osc::time time, const osc::bound_callback& cb)
{
    auto expired = osc::clock::now() - 100ms;
    timers_.erase(timers_.begin(), timers_.lower_bound(expired));

    asio::system_timer timer{ socket_.get_executor(), time };
    timer.async_wait([=](const asio::error_code& ec) { if(!ec) cb(); });

    timers_.emplace(time, std::move(timer));
}

////////////////////////////////////////////////////////////////////////////////
void server::cancel(const osc::message& msg)
{
    if(msg.values().are<osc::time>()) timers_.erase(msg.value(0).to_time());
}

////////////////////////////////////////////////////////////////////////////////
}
