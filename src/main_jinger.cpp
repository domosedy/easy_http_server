#include <iostream>
#include "jinger.hpp"


int main() {
    hashmap mapa;
    mapa["f"] = vec {1, 2, 3, 4, "ab"};
    mapa["i"] = "20.5";

    jinger jing;
    jing.open("/home/sielu/input.txt");
    auto val = jing.parse_file(mapa);

    std::cout << val.value_or(val.error()) << std::endl;
}