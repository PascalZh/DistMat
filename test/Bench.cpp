#include "Bench.hpp"

namespace test
{
string serialize(const Bench& bench)
{
  string str(bench.name + "\t " + to_string(bench.duration.count()) + "ms");
  return str;
}
} // namespace test