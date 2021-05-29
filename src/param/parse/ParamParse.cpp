#include "./ParamParse.hpp"

#include <iostream>

#include "./RunParamParse.hpp"

Param *ParamParse::parse(int argc, char *argv[]) {
  if (argc == 1) {
    std::cerr << "please use: " << argv[0] << " run --help" << std::endl;
    exit(-1);
  }

  if (argv[1] == string("run")) {
    return RunParamParse::parse(argc, argv);
  } else {
    std::cerr << "unsupport command: '" << argv[1] << "'" << std::endl;
    std::cerr << "please use: " << argv[0] << " run --help" << std::endl;
    exit(-1);
  }
}