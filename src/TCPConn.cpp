//
// Created by sielu on 7/23/23.
//

#include "TCPConn.hpp"
#include "html_parser.hpp"
#include "server_worker.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <fstream>
#include <algorithm>
#include <future>

connection::conn_ptr connection::make_new(io_service& service) {
    conn_ptr val(new connection(service));
    return val;
}

void connection::start() {
    if (!started) {
        started = true;
        do_read();
    }
}

void connection::stop() {
    if (started) {
        started = false;
        sock_.close();
    }
}

bool connection::is_started() const {
    return started;
}

ip::tcp::socket& connection::get_sock() {
    return sock_;
}

std::string connection::get_ip() {
    if (!started || !sock_.available()) {
        return "Bad request";
    }

    return sock_.remote_endpoint().address().to_string();
}

void connection::on_read(const connection::error_code& err, size_t transferred_bytes) {
    if (err) {
        process_data("");
        stop();
        return;
    }

    std::string msg(msg_read, transferred_bytes);
    auto self(shared_from_this());
    auto fut = std::async([&self, &msg]() {
        self->process_data(msg);
    });

    fut.wait();
}

void connection::do_read() {
    async_read(sock_, buffer(msg_read),
               transfer_at_least(32),
//               boost::bind(&connection::go_to_enter, shared_from_this(), _1, _2),
               boost::bind(&connection::on_read, shared_from_this(), _1, _2));
}

size_t connection::go_to_enter(const connection::error_code& err, size_t transferred_bytes) {
    if (err) {
        return 0;
    }

    bool found = std::find(msg_read, msg_read + transferred_bytes, '\n') < msg_read + transferred_bytes;
    return found ? 0 : 1;
}

void connection::on_write(const connection::error_code& err, size_t transferred_bytes) {
    if (err) {
        std::cout << err << std::endl;
        stop();
        return;
    }

    stop();
//    do_read();
}

void connection::do_write() {
    sock_.async_write_some(buffer(message),
                           boost::bind(&connection::on_write, shared_from_this(), _1, _2));
}

void connection::process_data(const std::string& msg) {
    if (msg == "exit\n") {
        stop();
        return;
    }

    std::future<html_query> fut = std::async(std::launch::async, parse_data, msg);
    fut.wait();
    auto val = fut.get();

    std::string type_of_request = val.type;
    int value = -1;

    if (val.type == "GET") {
        value = 1;
    }

    std::string answer;
    std::string str_file;

    auto f = std::async(std::launch::async, [&val, &value] () {
        val.parameters.insert({"name", val.url});
//    for (auto &it : val.parameters) {
//        std::cout << it.first << ' ' << it.second << std::endl;
//    }

        return (val.parameters.size() == 1) ?
               server_worker::get("file", value)
                       (val.parameters) :
               server_worker::get(val.url, value)(val.parameters);
    });
    f.wait();
//
    ret_type all_file = f.get();


    if (all_file.has_value()) {
        answer = "HTTP/1.1 200 OK\r\n";
        str_file = std::move(all_file.value());
    } else {
        answer = "HTTP/1.1 404 NOT FOUND\r\n";
        str_file = "<html> <body> <h1> 404 not found error </h1> </body> </html>";
    }


    answer += "Content-Length: " + std::to_string(str_file.size()) + "\r\n\r\n" + str_file;
    message = std::move(answer);

    do_write();
}