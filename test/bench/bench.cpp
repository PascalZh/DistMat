#include "bench.hpp"

namespace bench
{
string serialize(const Bench& bench)
{
  string str(bench.name + "\t " + to_string(bench.result.duration.count()) + "ms");
  return str;
}
} // namespace bench