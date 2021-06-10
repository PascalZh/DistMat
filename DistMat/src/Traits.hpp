#pragma once
#include <concepts>
/// Type traits for matrix and scalar.

namespace distmat {
namespace traits {
  
template<typename Scalar>
  struct scalar_traits {
    static_assert(!std::is_same_v<Scalar, Scalar>, "no scalar_traits for typename Scalar");
  };

template<typename Scalar>
  requires std::is_arithmetic_v<std::remove_cvref_t<Scalar>>
  struct scalar_traits<Scalar> {
    static constexpr std::remove_cvref_t<Scalar> identity = 0;
  };

} // namespace traits
} // namespace distmat