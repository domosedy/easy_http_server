//
// Created by sielu on 7/23/23.
//

#ifndef BOOSTSERVER_TCPCONN_H
#define BOOSTSERVER_TCPCONN_H

#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

using namespace boost::asio;

class connection : public boost::enable_shared_from_this<connection>, public boost::noncopyable {

public:

    typedef boost::shared_ptr<connection> conn_ptr;
    typedef boost::system::error_code error_code;

    static conn_ptr make_new(io_service& service);

    void start();
    void stop();

    bool is_started() const;

    ip::tcp::socket& get_sock();

private:

    std::string get_ip();

    connection(io_service& service) : sock_(service), started(false) {}

    void on_read(const error_code& err, size_t transferred_bytes);
    void do_read();

    size_t go_to_enter(const error_code& err, size_t transferred_bytes);

    void on_write(const error_code& err, size_t transferred_bytes);
    void do_write();

    void process_data(const std::string& msg);

    ip::tcp::socket sock_;
    enum {msg_max = 2048};
    char msg_read[msg_max];
    std::string message;
    bool started;
};


#endif //BOOSTSERVER_TCPCONN_H
