#include <iostream>
#include "jinger.hpp"


int main() {
    hashmap mapa;
    vec val_1 {};

    for (int i = 0; i < 1e3; i++) {
        val_1.push_back(i);
    }

    mapa["f"] = val_1;

    jinger::jinger jing;
    jing.open("/home/sielu/index.html");
    auto val = jing.parse_file(mapa);

    std::cout << val.value_or(val.error()) << std::endl;
}