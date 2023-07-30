#include "TCPServer.hpp"

class singleton_server {

    singleton_server(io_service& service, int PORT) : tcp(service, PORT) {}

public:
    singleton_server() = delete;
    singleton_server(singleton_server&) = delete;

    singleton_server& operator=(const singleton_server&) = delete;

    ~singleton_server() {
        if (ptr) {
            delete ptr;
        }
    }

    static singleton_server* get_instance(io_service &service, const int PORT) {
        if (!ptr) {
            ptr = new singleton_server(service, PORT);
        }

        return ptr;
    }

    tcp_server& get_server() {
        return tcp;
    }

protected:
    static singleton_server* ptr;
    tcp_server tcp;
};

singleton_server* singleton_server::ptr = nullptr;

#include "server_worker.hpp"
#include "jinger.hpp"
#include <fstream>

inline bool file_exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

inline ret_type get_input(std::unordered_map<std::string, std::string> val) {
    std::string name = "/home/sielu" + val["name"];
//    std::cout << name << std::endl;
    if (name.back() == '/') {
        name += "index.html";
    }

    hashmap param;

    vec df;

    if (val.count("repeat") == 0) {
        val["repeat"] = "1";
    }

//    std::cout << 42 << std::endl;
//    std::cout << val["repeat"] << std::endl;

    for (int i = 0; i < stoi(val["repeat"]); i++) {
//        std::cout << i << std::endl;
        df.emplace_back(i);
    }

    param["f"] = std::move(df);


    jinger::jinger file(name);

    auto fut = std::async(std::launch::async, [&file, &param] () {
        return file.parse_file(param);
    });


    auto answer = std::move(fut.get());

    if (answer.has_value()) {
        return answer.value();
    } else {
        return std::unexpected(-1);
    }
}

int main() {
    server_worker::add_function("file", GET, get_input);
    server_worker::add_function("/index.html", GET, get_input);

    io_service service;
    tcp_server server(service, 8001);
    server.start();
}