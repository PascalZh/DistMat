#include <iostream>
#include "src/Matrix.h"
using namespace DistMat;

int main(int argc, char const *argv[])
{
  Matrix<int> A(3, 3);
  int a;
  A.addTo(a);
  std::cout << A.size();
  return 0;
}
