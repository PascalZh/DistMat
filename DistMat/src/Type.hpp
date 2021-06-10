#pragma once
#include <concepts>
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <ranges>
#include <cassert>

namespace distmat
{
using Index = size_t;
using std::same_as, std::convertible_to, std::derived_from;
using std::string, std::vector, std::to_string;
using std::make_tuple;
using std::cout, std::cin, std::endl;

namespace ranges = std::ranges;
namespace views = std::views;
} // namespace distmat
