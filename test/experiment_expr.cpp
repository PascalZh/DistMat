#include <iostream>
#include <tuple>
#include <string_view>

using namespace std;

using Expr = string_view;
struct Int {
  int value;
  Int(int x) : value(x) {}
  Int& operator=(tuple<Int&, int, Expr> e)
  {
    auto[x, c, s] = e;
    if (&x == this) {
      x.value += c;
    } else {
      this->value = x.value + c;
    }
    return *this;
  }
};

tuple<Int&, int, Expr> operator+(Int& x, int c) {
  return forward_as_tuple(x, c, "+");
}
int main(int argc, char const *argv[])
{
  Int x{1};
  x = x + 2;
  return 0;
}