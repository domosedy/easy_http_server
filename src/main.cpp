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

int main() {
    io_service service;
    auto wrapper = singleton_server::get_instance(service, 8001);
    wrapper->get_server().start();
}