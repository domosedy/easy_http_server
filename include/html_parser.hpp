//
// Created by sielu on 7/30/23.
//

#ifndef BOOSTSERVER_HTML_PARSER_HPP
#define BOOSTSERVER_HTML_PARSER_HPP

#include <string>
#include <unordered_map>

struct html_query {
    std::string type;
    std::string url;
    std::unordered_map<std::string, std::string> parameters;
};

html_query parse_data(std::string);


#endif //BOOSTSERVER_HTML_PARSER_HPP
