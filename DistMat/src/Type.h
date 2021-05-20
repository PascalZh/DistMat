#pragma once
#include <concepts>

namespace DistMat
{
using Index = size_t;
using std::same_as, std::convertible_to, std::derived_from;
using std::string, std::vector, std::to_string;
using std::make_tuple;

namespace ranges = std::ranges;
namespace views = std::views;
} // namespace DistMat