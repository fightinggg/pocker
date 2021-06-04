#include "./param_parse.hpp"

#include <iostream>

#include "./run_param_parse.hpp"

param *param_parse::parse(int argc, char *argv[]) {
  if (argc == 1) {
    std::cerr << "please use: " << argv[0] << " run --help" << std::endl;
    exit(-1);
  }

  if (argv[1] == string("run")) {
    return run_param_parse::parse(argc, argv);
  } else {
    std::cerr << "unsupport command: '" << argv[1] << "'" << std::endl;
    std::cerr << "please use: " << argv[0] << " run --help" << std::endl;
    exit(-1);
  }
}