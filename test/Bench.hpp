#pragma once
#include "BasicTest.hpp"

namespace test
{

struct Bench : Test {
  chrono::duration<double, std::milli> duration;
};

inline vector<Bench> allBenches; // NOLINT

// NOLINTNEXTLINE
#define BENCH(name_, desc_, cnt_, codes_) \
  {\
    Bench bench;\
    bench.name = name_;\
    bench.description = desc_;\
    bench.count = cnt_;\
    bench.sourceCodes = #codes_;\
    allBenches.push_back(std::move(bench));\
    auto bench_start = chrono::steady_clock::now();\
    codes_\
    allBenches.back().duration = chrono::steady_clock::now() - bench_start;\
    cout << serialize(allBenches.back()) << endl;\
  }

string serialize(const Bench& bench);

} // namespace test