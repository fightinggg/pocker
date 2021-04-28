#include <string>
#include <iostream>
#include "../../utils/ObjUtils.hpp"

class ParseUtils {
public:
    static int parseMemory(std::string memory) {
        if (memory.back() != 'm') {
            std::cerr << "error flag memory: " + memory << std::endl;
            exit(-1);
        }
        memory.pop_back();
        return ObjUtils::stringToInt(memory);
    }
};



