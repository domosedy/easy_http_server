#ifndef TCPServer_BOOST
#define TCPServer_BOOST

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;

class connection;

typedef boost::shared_ptr<connection> conn_ptr;
typedef boost::system::error_code error_code;

struct tcp_server : boost::noncopyable {

    tcp_server(io_service& service, const int PORT) : service(&service), acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), PORT)) {}

    void start();

private:

    void handle_connect(conn_ptr client, const error_code &err);

    ip::tcp::acceptor acceptor;
    std::unique_ptr<io_service> service;
};

#endif