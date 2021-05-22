#include <iostream>
#include "src/Matrix.h"
#include "bench/bench.h"
using namespace DistMat;
using std::cout, std::cin, std::endl;

template<typename Mat>
void test_add_eq_mul(Mat& A, int cnt)
{
  Mat B = A;
  BENCH("add_eq_mul", cnt, "A + A + ... + A is equivalent to n * A",
    for (int i = 0; i < cnt; ++i)
      A = A + B;
  );
  if (A != (cnt + 1) * B)
    throw make_tuple(allBenches.back(), A, B);
}

template<typename Mat>
void test_sub_eq_mul(Mat& A, int cnt)
{
  Mat B = A;
  A = A * cnt;
  BENCH("sub_eq_mul", cnt, "substract A multiple times is equivalent to substract n * A",
    for (int i = 0; i < cnt; ++i)
      A = A - B;
  );
  if (A != Mat(A.rows(), A.cols())/*TODO this constructor should not be zero-initialized*/)
    throw make_tuple(allBenches.back(), A, B);
}

int main(int argc, char const *argv[])
{
  Matrix<int> A(3, 3);
  A(0, 0) = 2;

  test_add_eq_mul(A, 10);
  test_sub_eq_mul(A, 10);
  cout << A(0, 0) << endl;
  cout << A(0, 2) << endl;
  cout << "hello" << endl;
  cout << A.size() << endl;
  cout << A.rows() << endl;
  return 0;
}
