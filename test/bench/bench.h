#pragma once
#include <string>
#include <functional>
#include <chrono>

using std::string;
using std::function;
using std::vector;
namespace chrono = std::chrono;

struct Environment {
  string os;
  string cpu;
  string date;
};

struct BenchResult {
  Environment env;
  chrono::seconds duration;
  bool isFailed = false;
};

struct Bench {
  string name;
  string benchCodesContent;
  size_t iterations;
  string description;

  BenchResult result;
};

void runBench(Bench& bench);