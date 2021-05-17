#include <iostream>
#include "src/Matrix.h"
#include "bench/bench.h"
using namespace DistMat;
using std::cout, std::cin, std::endl;

int main(int argc, char const *argv[])
{
  Matrix<int> A(3, 3);
  A(0, 0) = 2;
  Matrix<int> B(3, 3);
  B(0, 0) = 3;
  BENCH("add", 1, "add and assign",
        A = A + B;
        A = A * 5;)

  cout << A(0, 0) << endl;
  cout << A(0, 2) << endl;
  cout << "hello" << endl;
  cout << A.size() << endl;
  cout << A.rows() << endl;
  return 0;
}
