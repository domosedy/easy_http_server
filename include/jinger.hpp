//
// Created by sielu on 7/30/23.
//

#ifndef BOOSTSERVER_JINGER_HPP
#define BOOSTSERVER_JINGER_HPP

#include <unordered_map>
#include <any>
#include <boost/noncopyable.hpp>
#include <string>
#include <expected>
#include <vector>
#include <variant>
#include <memory>

#define vec std::vector<std::any>
#define hashmap std::unordered_map<std::string, std::any>

namespace jinger {

    using vals = std::variant<std::string, int, double>;
    using ret_type = std::expected<vals, std::string>;
    using ret_type_str = std::expected<std::string, std::string>;

    class jinger : public boost::noncopyable {
    public:

        jinger() = default;

        jinger(const std::string &file_name);

        void open(const std::string &file_name);

        ret_type_str parse_file(std::unordered_map<std::string, std::any> &) const;

    private:

        inline ret_type to_ret_type(std::any &) const;

        inline ret_type_str to_string(ret_type &) const;

        inline ret_type get_variable(std::string &, const std::unordered_map<std::string, std::any> &) const;

        inline ret_type_str parse_string(size_t, size_t, const std::vector<std::string> &, const std::vector<size_t> &,
                                         std::unordered_map<std::string, std::any> &) const;

        inline ret_type
        get_variable(std::vector<std::string> &, const std::unordered_map<std::string, std::any> &) const;

        std::string file_name;
    };

}


#endif //BOOSTSERVER_JINGER_HPP
