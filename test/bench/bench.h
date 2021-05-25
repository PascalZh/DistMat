#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <iostream>

using std::string, std::to_string, std::vector, std::cout, std::endl, std::cin;
namespace chrono = std::chrono;

struct Bench;
inline vector<Bench> allBenches;

struct Environment {
  string os;
  string cpu;
  string date;
};

struct BenchResult {
  Environment env;
  chrono::duration<double, std::milli> duration;
  bool isFailed = false;
};

struct Bench {
  string name;
  size_t iterations;
  string description;
  string benchCodesContent;

  BenchResult result;
};

#define BENCH(name, iterations, desc, codes) \
  {\
    allBenches.emplace_back(name, iterations, desc, #codes, BenchResult{});\
    auto bench_start = chrono::steady_clock::now();\
    codes\
    allBenches.back().result.duration = chrono::steady_clock::now() - bench_start;\
    cout << bench::serialize(allBenches.back()) << endl;\
  }

namespace bench {
  string serialize(const Bench& bench);
} // namespace bench