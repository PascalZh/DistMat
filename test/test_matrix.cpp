#include <iostream>
#include "DistMat/src/Matrix.hpp"
#include "bench/bench.hpp"
using namespace DistMat;
using namespace bench;

template<typename Mat>
void test_add_eq_mul(Mat& A, int cnt)
{
  Mat B = A;
  BENCH("m:add_eq_mul", 1, "A + A + ... + A is equivalent to n * A",
    for (int i = 0; i < cnt; ++i) {
      A = A + B;
    }
  )
  if (A != (cnt + 1) * B) {
    throw make_tuple(allBenches.back(), A, B);
  }
}

template<typename Mat>
void test_sub_eq_mul(Mat& A, int cnt)
{
  Mat B = A;
  A = A * cnt;
  BENCH("m:sub_eq_mul", 1, "substract A multiple times is equivalent to substract n * A",
    for (int i = 0; i < cnt; ++i) {
      A = A - B;
    }
  )
  Mat C(A.rows(), A.cols());
  for (size_t i = 0; i < C.size(); ++i) {
    cout << C[i] << endl;
  }
  
  if (A != Mat(A.rows(), A.cols())) {//TODO use Matrix::Zero
    return;
    throw make_tuple(allBenches.back(), A, B);
  }
}

template<typename Mat>
void test_at_vs_operator_paren_speed(const Mat& A, int cnt)
{
  // TODO: figure out why vector is faster than raw array
  double sum = 0.0;
  vector<typename Mat::scalar_type> B(A.size());
  BENCH("m:element_access:at", cnt, "test how fast is A.at(i, j)",
    for (int it = 0; it < cnt; ++it) { 
      for (Index i = 0; i < A.rows(); ++i) {
        for (Index j = 0; j < A.cols(); ++j) {
          sum += A.at(i, j);
        }
      }
    }
  )

  // sum must be used, otherwise the following codes will not compile
  BENCH("m:element_access:operator()", cnt, "test how fast is A(i, j)",
    for (int it = 0; it < cnt; ++it) { 
      for (Index i = 0; i < A.rows(); ++i) {
        for (Index j = 0; j < A.cols(); ++j) {
          sum += A(i, j);
        }
      }
    }
  )

  cout << "test_at_vs_operator_paren_speed: " << sum << endl;
}

void test_default_init()
{
  int cnt = 1000;
  BENCH("m:default_init:constructor", cnt, "test if matrix is allocated but not initialized: use constructor Mat(Index rows, Index cols)",
    for (int i = 0; i < cnt; ++i) {
      Matrix<int> A(100, 100);
    }
  )

  BENCH("m:default_init:vector", cnt, "test if matrix is allocated but not initialized: use vector(size_t n)",
    for (int i = 0; i < cnt; ++i) {
      vector<int> B(10000);
    }
  )
  constexpr size_t N = 3;
  Matrix<int> C(N, N);
  for (size_t i = 0; i < C.size(); ++i) {
    cout << "C[i] = " << C[i] << " with i = " << i << endl;
  }
}

int main(int argc, char const *argv[])
{
  int* p = new int; // NOLINT
  cout << *p << endl; // NOLINT
  p = new int[10000000]; // NOLINT
  cout << p[10000000 - 2] << endl; // NOLINT

  Matrix<int> A(3, 3);
  A(0, 0) = 2;
  A(0, 1) = 3;
  A(0, 2) = 4;
  Matrix<int> B(3, 3);
  B = -A;

  test_add_eq_mul(A, 10);
  test_sub_eq_mul(A, 10);
  test_at_vs_operator_paren_speed(B, 1000 * 1000);
  test_default_init();
  
  cout << A(0, 0) << endl;
  cout << A(0, 2) << endl;
  cout << "hello" << endl;
  cout << A.size() << endl;
  cout << A.rows() << endl;
  cout << B(0, 0) << endl;
  B(1, 2) = 4;
  cout << B[5] << endl;
  return 0;
}
