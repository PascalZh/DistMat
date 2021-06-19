#pragma once
// Third party
#include "range.hpp"

#include <vector>
#include <utility>

namespace distmat
{
namespace util
{
using ::util::lang::range;
using ::util::lang::indices;

// Allocator adaptor that interposes construct() calls to
// convert value initialization into default initialization.
template <typename T, typename A=std::allocator<T>>
class default_init_allocator : public A {
  typedef std::allocator_traits<A> a_t;
public:
  template <typename U> struct rebind {
    using other =
      default_init_allocator<
        U, typename a_t::template rebind_alloc<U>
      >;
  };

  using A::A;

  template <typename U>
  void construct(U* ptr)
    noexcept(std::is_nothrow_default_constructible<U>::value) {
    ::new(static_cast<void*>(ptr)) U;
  }
  template <typename U, typename...Args>
  void construct(U* ptr, Args&&... args) {
    a_t::construct(static_cast<A&>(*this),
                   ptr, std::forward<Args>(args)...);
  }
};

namespace detail {
  template<class T, T... Is, class F>
  constexpr void loop(std::integer_sequence<T, Is...>, F&& f) {
    (f(std::integral_constant<T, Is>{}), ...);// C++17 fold expression
  }
}// detail

template<class T, T count, class F>
constexpr void loop(F&& f) {
  detail::loop(std::make_integer_sequence<T, count>{}, std::forward<F>(f));
}

template<class> inline constexpr bool always_false_v = false;

} // namespace util
} // namespace distmat
