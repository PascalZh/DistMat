#include <Eigen/Dense>
#include <iostream>
#include <tuple>
#include <string_view>
#include <vector>
#include <string>

using namespace std;

class Foo {
  virtual void foo() = 0;
};

template<class T>
class SingletonHolder final {
public:
  static T& GetInstance()
  {
    static T instance;
    return instance;
  }

  SingletonHolder() = delete;
  ~SingletonHolder() = delete;
  SingletonHolder(const SingletonHolder&) = delete;
  SingletonHolder(SingletonHolder&&) = delete;
  SingletonHolder& operator=(const SingletonHolder&) = delete;
  SingletonHolder& operator=(SingletonHolder&&) = delete;
};

void Foo::foo() {
  cout << "foo" << endl;
}

class BarImpl final : public Foo {
public:
  void foo() override { cout << "bar" << endl; }
};

using Bar = SingletonHolder<BarImpl>;

int main(int argc, char const *argv[])
{
  BarImpl& b = Bar::GetInstance();
  b.foo();
  return 0;
}