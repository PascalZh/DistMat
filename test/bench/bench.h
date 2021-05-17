#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <vector>

using std::string;
using std::function;
using std::vector;
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
  allBenches.emplace_back(name, iterations, desc, #codes, BenchResult{});\
  auto bench_start = chrono::steady_clock::now();\
  codes\
  allBenches.back().result.duration = chrono::steady_clock::now() - bench_start;\
  std::cout << allBenches.back().result.duration.count() << "ms" << std::endl;
