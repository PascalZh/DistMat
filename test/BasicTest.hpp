#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <iostream>

namespace test
{
  
using std::string, std::to_string;
using std::vector;
namespace chrono = std::chrono;

struct Env {
  string os;
  string cpu;
  string date;
};

struct Test {
  string  name;
  string  description;

  size_t  count = 1;
  Env     env;
  string  sourceCodes;
  bool    isSucceeded = true;
};

} // namespace test