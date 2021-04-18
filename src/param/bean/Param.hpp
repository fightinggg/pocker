#pragma once

#include "./ParamType.hpp"


class Param
{
private:
    ParamType paramType;

public:
   ParamType getParamType(){
       return paramType;
   }

   void setParamType(ParamType paramType){
       this->paramType = paramType;
   }
};