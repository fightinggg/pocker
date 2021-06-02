#pragma once

#include <string>
#include <vector>

#include "./Param.hpp"

using namespace std;

class RunParam : public Param {
 private:
  bool tty;
  bool interactive;
  bool detach;
  int memory;      // bytes
  int memorySwap;  // bytes
  int disk;

  // Mb
  double cpus;  // core numbers
  string image;
  std::vector<string> exec;

  string containerId;
  string containerName;

 public:
  const string &getContainerId() const { return containerId; }

  void setContainerId(const string &containerId) {
    RunParam::containerId = containerId;
  }

  const string &getContainerName() const { return containerName; }

  void setContainerName(const string &containerName) {
    RunParam::containerName = containerName;
  }

 public:
  int getMemory() const { return memory; }

  void setMemory(int memory) { RunParam::memory = memory; }

  int getMemorySwap() const { return memorySwap; }

  void setMemorySwap(int memorySwap) { RunParam::memorySwap = memorySwap; }

  double getCpus() const { return cpus; }

  void setCpus(double cpus) { RunParam::cpus = cpus; }
  
  int getDisk() const { return disk; }

  void setDisk(int disk) { this->disk = disk; }

 public:
  bool isTty() const { return tty; }

  void setTty(bool tty) { RunParam::tty = tty; }

  bool isInteractive() const { return interactive; }

  void setInteractive(bool interactive) { RunParam::interactive = interactive; }

  bool isDetach() const { return detach; }

  void setDetach(bool detach) { RunParam::detach = detach; }

  const string &getImage() const { return image; }

  void setImage(const string &image) { RunParam::image = image; }

  const vector<string> &getExec() const { return exec; }

  void setExec(const vector<string> &exec) { RunParam::exec = exec; }

  string toString() {
    return string("RunParam:: ") + "tty: " + std::to_string(tty) + ", " +
           "interactive: " + std::to_string(interactive) + ", " +
           "detach: " + std::to_string(detach) + ", " +
           "memory: " + std::to_string(memory) + ", " +
           "memorySwap: " + std::to_string(memorySwap) + ", " +
           "cpus: " + std::to_string(cpus) + ", " + "image: " + image + "," +
           "containerId: " + containerId + "," +
           "containerName: " + containerName + "";
  }
};