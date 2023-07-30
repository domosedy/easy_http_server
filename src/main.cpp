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

    for (auto &it : val) {
        param[it.first] = it.second;
    }


    jinger::jinger file(name);

    auto answer = file.parse_file(param);

    if (answer.has_value()) {
        return answer.value();
    } else {
        return std::unexpected(-1);
    }
}

int main() {
    server_worker::add_function("file", GET, get_input);
//
//    auto val = server_worker::get("file", GET)(std::unordered_map<std::string, std::string> {
//        std::pair<std::string, std::string> {"name", "/inex.html"}
//    });
//
//    std::cout << val.value_or("-12") << std::endl;
//    return 0;

    io_service service;
    auto wrapper = singleton_server::get_instance(service, 8001);
    wrapper->get_server().start();
}