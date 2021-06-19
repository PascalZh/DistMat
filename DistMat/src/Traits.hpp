#pragma once
#include "Util.hpp"
#include <concepts>
/// Type traits for matrix and scalar.

namespace distmat {
namespace traits {
  
template<typename Scalar>
  struct scalar_traits {
    static_assert(util::always_false_v<Scalar>, "no scalar_traits for typename Scalar");
  };

template<typename Scalar>
  requires std::is_arithmetic_v<std::remove_cvref_t<Scalar>>
  struct scalar_traits<Scalar> {
    static constexpr std::remove_cvref_t<Scalar> zero = 0;
    static constexpr std::remove_cvref_t<Scalar> one = 1;
  };

} // namespace traits
} // namespace distmat