//
// Created by sielu on 7/30/23.
//

#ifndef BOOSTSERVER_SERVER_WORKER_HPP
#define BOOSTSERVER_SERVER_WORKER_HPP

#include <string>
#include <boost/bind.hpp>
#include <unordered_map>
#include <map>
#include <any>
#include <expected>
#include <iostream>

using ret_type = std::expected<std::string, int>;
using any_callable = std::function<ret_type(std::unordered_map<std::string, std::string>)>;

#define GET 1
#define POST 2
#define PUT 4
#define DELETE 8

struct server_worker {

    static void add_function(const std::string& str, int methods, any_callable f) {
        for (int i = 0; i < 4; i++) {
            if (methods & (1 << i)) {
                function_storer[{str, (1 << i)}] = f;
            }
        }
    }

    static void add_function(const std::vector<std::string> str, int methods, any_callable f) {
        for (auto &it : str) {
            add_function(it, methods, f);
        }
    }

    static inline any_callable get(const std::string& str, int method) {
        if (!function_storer.contains({str, method})) {
            return any_callable( [] (std::unordered_map<std::string, std::string>) {
                return std::unexpected(-1);
            });
        }


        return function_storer[{str, method}];
    }

private:
    static inline std::map<std::pair<std::string, int>, any_callable> function_storer;
};

#endif //BOOSTSERVER_SERVER_WORKER_HPP
