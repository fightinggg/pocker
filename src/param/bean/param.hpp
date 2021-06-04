#pragma once

#include "./param_type.hpp"

class Param {
 private:
  param_type paramType;

 public:
  param_type getParamType() { return paramType; }

  void setParamType(param_type paramType) { this->paramType = paramType; }
};