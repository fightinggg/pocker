#include <iostream>
#include <string>

#include "../../utils/obj_utils.hpp"

class parse_utils {
 public:
  static int parse_memory(std::string memory) {
    if (memory.back() != 'm') {
      std::cerr << "error flag memory: " + memory << std::endl;
      std::cerr << "please use flag '--memory 10m' " << std::endl;
      exit(-1);
    }
    memory.pop_back();
    return obj_utils::string_to_int(memory) << 20;
  }
};
