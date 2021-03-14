////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "server.hpp"
#include "system.hpp"

#include <exception>
#include <functional> // std::bind
#include <iostream>
#include <memory>

using namespace std::placeholders;

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
server::server(asio::io_context& io, const udp::endpoint& local, const src::actions& actions) :
    socket_{ io }, space_{ std::bind(&server::callback_sched, this, _1, _2) }
{
    set_child_exit_callback([](pid_t pid, int status)
    {
        std::cout << "Process " << pid << " exited with status " << status << std::endl;
    });

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
    auto& io = socket_.get_io_context();
    auto timer = std::make_shared<asio::system_timer>(io, time);

    timer->async_wait([timer, cb](const asio::error_code& ec) { if(!ec) cb(); });
}

////////////////////////////////////////////////////////////////////////////////
}
