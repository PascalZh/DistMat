#include <iostream>
#include "src/Matrix.h"
using namespace DistMat;
using std::cout, std::cin, std::endl;

int main(int argc, char const *argv[])
{
  Matrix<int> A(3, 3);
  A(0, 0) = 2;
  Matrix<int> B(3, 3);
  B(0, 0) = 3;
  A = A + B;
  cout << A(0, 0) << endl;
  cout << A(0, 2) << endl;
  return 0;
}
