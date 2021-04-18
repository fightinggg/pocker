#include "./ParamParse.hpp"
#include "./RunParamParse.hpp"
#include <iostream>

Param *ParamParse::parse(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "please run:\n      " << argv[0] << " run --help" << std::endl;
        exit(-1);
    }

    if (argv[1] == string("run")) {
        return RunParamParse::parse(argc, argv);
    } else {
        std::cerr << "unknow error in file ParamParse" << std::endl;
        std::cerr << "your command: '" << argv[1] << "'" << std::endl;
        exit(-1);
    }
}