#pragma once

#include <string>
#include <vector>

#include "./param.hpp"

using namespace std;

class volume_mapping {
 public:
  string from;
  string to;
  int type;
};

class run_param : public param {
 public:
  bool tty;
  bool interactive;
  bool detach;
  int memory;      // bytes
  int memory_swap;  // bytes
  int disk;

  // Mb
  double cpus;  // core numbers
  string image;
  vector<string> exec;
  vector<volume_mapping> volumes;

  string id;
  string name;

  string tostring() {
    return string("run_param:: ") + "tty: " + std::to_string(tty) + ", " +
           "interactive: " + std::to_string(interactive) + ", " +
           "detach: " + std::to_string(detach) + ", " +
           "memory: " + std::to_string(memory) + ", " +
           "memory_swap: " + std::to_string(memory_swap) + ", " +
           "cpus: " + std::to_string(cpus) + ", " + "image: " + image + "," +
           "id: " + id + "," +
           "name: " + name + "";
  }
};