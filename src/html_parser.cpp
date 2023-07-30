//
// Created by sielu on 7/30/23.
//

#include "html_parser.hpp"

#include <sstream>
#include <iostream>
#include <algorithm>

html_query parse_data(std::string msg) {
    html_query asker;

    std::stringstream ss(msg);

    ss >> asker.type;

    std::string url_query;
    ss >> url_query;
    std::cout << url_query << std::endl;
    

    auto url_end_iterator = std::find(url_query.begin(), url_query.end(), '?');
//    return asker;
    asker.url = std::string(url_query.begin(), url_end_iterator);
//    return asker;
    if (url_end_iterator != url_query.end()) {
        url_end_iterator++;
    }

    url_query.erase(url_query.begin(), url_end_iterator);

    std::cout << asker.url << ": " << url_query << std::endl;

    if (url_query.size() == 0) {
        return asker;
    }

    std::string current_word = "";
    std::string prev_word = "";
    bool is_first = true;

    url_query += '&';

    for (const char &it : url_query) {
        if (it == '&') {
            if (prev_word.empty() || current_word.empty()) {
                asker.url = "/404.html";
                return asker;
            }

            asker.parameters[prev_word] = std::move(current_word);

            is_first = true;
        } else if (it == '=') {
            is_first = false;
        } else if (is_first) {
            prev_word.push_back(it);
        } else {
            current_word.push_back(it);
        }
    }

    for (auto &it : asker.parameters) {
        std::cout << it.first << ": " << it.second << std::endl;
    }

    return asker;
}