//
// Created by sielu on 7/30/23.
//

#include "jinger.hpp"
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>

#define err(v) ::std::unexpected(v)

namespace jinger {

    inline ret_type_str jinger::to_string(ret_type &val) const {
        if (!val.has_value()) {
            return err("error in formatting");
        }

        auto val2 = val.value();

        try {
            auto w = std::get<int>(val2);
            return std::to_string(w);
        } catch (const std::bad_variant_access &ex) {}

        try {
            auto w = std::get<double>(val2);
            return std::to_string(w);
        } catch (const std::bad_variant_access &ex) {}

        try {
            auto w = std::get<std::string>(val2);
            return w;
        } catch (const std::bad_variant_access &ex) {}

        return err("error in formatting");
    }

    inline ret_type jinger::to_ret_type(std::any &val) const {
        try {
            return std::any_cast<int>(val);
        } catch (std::bad_any_cast &e) {}

        try {
            return std::any_cast<double>(val);
        } catch (std::bad_any_cast &e) {}

        try {
            return std::any_cast<const char *>(val);
        } catch (std::bad_any_cast &e) {}

        return err("error in formatting");
    }

    inline bool file_exists(const std::string &name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    jinger::jinger(const std::string &file_name) {
        if (!file_exists(file_name)) {
            return;
        }

        this->file_name = std::move(file_name);
    }

    void jinger::open(const std::string &file_name) {
        if (!file_exists(file_name)) {
            return;
        }

        this->file_name = std::move(file_name);
    }

    ret_type_str jinger::parse_file(std::unordered_map<std::string, std::any> &map) const {
        if (file_name.empty()) {
            return err("Empty input file");
        }

        std::string current_line;
        std::vector<std::string> all_words;

        std::ifstream input(file_name);

        while (!input.eof()) {
            getline(input, current_line);
            std::string current_word = "";

            std::stringstream ss(current_line);

            while (ss >> current_word) {
                all_words.emplace_back(std::move(current_word));
            }

            all_words.push_back("\n");
        }

        std::vector<int> value;
        std::vector<size_t> pair_position(all_words.size(), -1);


        for (int i = 0; i < all_words.size(); i++) {
            if (all_words[i] == "{{" && (i + 2 >= all_words.size() || all_words[i + 2] != "}}")) {
                return err("Error in matching of {{ brackets");
            }

            if ((all_words[i] == "{%" && (
                    (i + 2 >= all_words.size() || all_words[i + 2] != "%}") &&
                    (i + 5 >= all_words.size() || all_words[i + 5] != "%}")
            ))) {
                return err("Error in matching of {% brackets");
            }
        }

        for (int i = 0; i + 1 < all_words.size(); i++) {
            if (all_words[i] != "{%") {
                continue;
            }

            if (all_words[i + 1] == "for") {
                value.push_back(i + 1);
            } else if (all_words[i + 1] == "endfor") {
                if (value.size() == 0) {
                    return err("Error in matching for and endfor");
                }

                pair_position[value.back()] = i + 1;
                value.pop_back();
            } else {
                return err("Unknown key word in " + all_words[i + 1] + " word");
            }
        }

//    return "12";
        if (value.size() != 0) {
            return err("Error in matching of for and endfor");
        }

        auto val = parse_string(0, all_words.size(), all_words, pair_position, map);

        if (val.has_value()) {
            return val.value();
        } else {
//        std::cout << val.err() << std::endl;
            return err(val.error());
        }
    }

    ret_type_str jinger::parse_string(size_t first_pos,
                                      size_t last_pos,
                                      const std::vector<std::string> &all_words,
                                      const std::vector<size_t> &all_position,
                                      std::unordered_map<std::string, std::any> &map) const {
        std::string my_answer = "";

        for (size_t i = first_pos; i + 2 < last_pos; i++) {
            if (all_words[i] == "{{") {
                std::string word = all_words[i + 1];
                ret_type val = get_variable(word, map);

                if (val.has_value()) {
                    my_answer += to_string(val).value() + ' ';
                } else {
                    return err("Error in getting value in " + all_words[i + 1] + " value");
                }

                i += 2;

            } else if (all_words[i] == "{%") {
                if (all_words[i + 1] == "for") {

                    std::string val_name = std::move(all_words[i + 2]);
                    std::string iter_variable = std::move(all_words[i + 4]);

                    if (!map.contains(iter_variable) || all_words[i + 3] != "in" || all_words[i + 5] != "%}") {
                        return err("Error in value of " + iter_variable + " in for cycle");
                    }

                    std::vector<std::any> val;

                    try {
                        val = std::any_cast<std::vector<std::any>>(map.at(iter_variable));
                    } catch (std::bad_any_cast &e) {
                        return err("Iterating value is not a vector");
                    }

                    std::optional<std::any> backup_val = std::nullopt;

                    if (map.contains(val_name)) {
                        backup_val = map.at(val_name);
                    }

                    for (auto &it: val) {
                        map[val_name] = it;

                        auto val = parse_string(i + 6, all_position[i + 1] - 1, all_words, all_position, map);
                        if (!val.has_value()) {
                            return err(val.error());
                        }

                        my_answer += val.value();
                    }

                    if (backup_val.has_value()) {
                        map[val_name] = backup_val.value();
                    } else {
                        map.erase(val_name);
                    }

                    i = all_position[i + 1] + 1;
                } else {
                    return err("Unknow key word");
                }

            } else {
                my_answer += all_words[i] + ' ';

                if (all_words[i] == "\n") {
                    my_answer.pop_back();
                }
            }
        }

        return my_answer;
    }

    ret_type jinger::get_variable(std::string &word, const std::unordered_map<std::string, std::any> &map) const {
        word += ".";
        std::vector<std::string> recursive_words;

        bool is_digit = 1;

        std::string current_word;
        for (auto &it: word) {
            if (it == '.') {
                recursive_words.emplace_back(std::move(current_word));
                current_word.clear();
            } else {
                is_digit &= isdigit(it);
                current_word.push_back(it);
            }
        }


        if (is_digit && recursive_words.size() <= 2) {
            word.pop_back();
//        std:: << "# " << word << std::endl;
            return word;
        }

        std::reverse(recursive_words.begin(), recursive_words.end());

        ret_type val = get_variable(recursive_words, map);
        return val;
    }

    ret_type jinger::get_variable(std::vector<std::string> &recursive_words,
                                  const std::unordered_map<std::string, std::any> &all_variables) const {
        if (!all_variables.contains(recursive_words.back())) {
            return err("12");
        }

        if (recursive_words.size() == 1) {
            try {
                auto val = all_variables.at(recursive_words.back());
                return to_ret_type(val);
            } catch (const std::bad_any_cast &e) {
//            std:: << 42 << std::endl;
                return err("12");
            }
        }

        std::string value = std::move(recursive_words.back());
        recursive_words.pop_back();

        try {
            auto val = all_variables.at(value);
            return get_variable(recursive_words, std::any_cast<std::unordered_map<std::string, std::any>>(val));
        } catch (const std::bad_any_cast &e) {
            return err("12");
        }
    }

}