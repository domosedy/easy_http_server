#include "TCPServer.hpp"
#include "TCPConn.hpp"

#include <iostream>
#include <boost/bind.hpp>

void tcp_server::start() {
    auto new_client = connection::make_new(*service);
    acceptor.async_accept(new_client->get_sock(),
              boost::bind(&tcp_server::handle_connect, this, new_client, _1));
    service->run();
}

void tcp_server::run(conn_ptr client) {
    client->start();
    service->run();
}

void tcp_server::handle_connect(conn_ptr client, const error_code &err) {
    mine.push_back(std::async( [this, client] () { run(client); }));

    auto new_client = connection::make_new(*service);

    acceptor.async_accept(new_client->get_sock(),
                          boost::bind(&tcp_server::handle_connect, this, new_client, _1));
}
