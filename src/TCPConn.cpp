//
// Created by sielu on 7/23/23.
//

#include "TCPConn.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <fstream>
#include <algorithm>

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

//    while (!sock_.available());

    return sock_.remote_endpoint().address().to_string();
}

void connection::on_read(const connection::error_code& err, size_t transferred_bytes) {
    if (err) {
        process_data("");
//        stop();
        return;
    }

    std::string msg(msg_read, transferred_bytes);
    process_data(msg);
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

    do_read();
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

    std::string first_line = "";

    for (const char &it : msg) {
        if (it == '\n') {
            break;
        }

        first_line += it;
    }

    std::stringstream ss(first_line);
    std::string type_of_request;
    ss >> type_of_request;

    std::string query = "";
    std::string current_word;

    while (ss >> current_word) {
        if (current_word == "HTTP/1.1") {
            break;
        }

        query += current_word + ' ';
    }

    query.pop_back();
    std::cout << type_of_request << '\n' << query << std::endl;

    std::string file_name = "";
    if (type_of_request == "GET") {
        for (const char &it : query) {
            if (it == '?') {
                break;
            }
            file_name += it;
        }
    } else {
        message = "Fuck your mom";
        do_write();
        return;
    }

    if (file_name == "/") {
        file_name += "/index.html";
    }

    std::cout << "Message from " << get_ip() << ": " << msg << std::endl;
    file_name = "/home/sielu" + file_name;

    std::ifstream input_html(file_name);


    std::string answer = "";

    std::string all_file = "";
    if (input_html.good()) {
        std::string line;
        answer = "HTTP/1.1 200 OK\r\n";

        while (input_html >> line) {
            all_file += line + ' ';
        }
        all_file.pop_back();
    } else {
        answer = "HTTP/1.1 404 NOT FOUND";
        all_file += "<p> 404 not FOUND Error </p>";
    }


    answer += "Content-Length: " + std::to_string(all_file.size()) + "\r\n\r\n" + all_file;
    message = std::move(answer);

    do_write();
}