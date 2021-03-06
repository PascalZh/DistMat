#include <iostream>
#include "DistMat/src/Matrix.hpp"
#include "Bench.hpp"
using namespace distmat;
using namespace test;

template<typename Mat>
void test_add_eq_mul(Mat& A, int cnt)
{
  Mat B = A;
  BENCH("m:add_eq_mul", "A + A + ... + A is equivalent to n * A", 1,
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
  BENCH("m:sub_eq_mul", "substract A multiple times is equivalent to substract n * A", 1,
    for (int i = 0; i < cnt; ++i) {
      A = A - B;
    }
  )
  Mat C(A.rows(), A.cols());
  for (size_t i = 0; i < C.size(); ++i) {
    cout << C[i] << endl;
  }

  if (A != Mat::zeros(A.rows(), A.cols())) {
    throw make_tuple(allBenches.back(), A, B);
  }
}

template<typename Mat>
void test_at_vs_operator_paren(const Mat& A, int cnt)
{
  double sum = 0.0;
  BENCH("m:element_access:at", "test how fast is A.at(i, j)", cnt,
    for (int it = 0; it < cnt; ++it) { 
      for (Index i = 0; i < A.rows(); ++i) {
        for (Index j = 0; j < A.cols(); ++j) {
          sum += A.at(i, j);
        }
      }
    }
  )

  // sum must be used, otherwise the following codes will not compile
  BENCH("m:element_access:operator()", "test how fast is A(i, j)", cnt,
    for (int it = 0; it < cnt; ++it) { 
      for (Index i = 0; i < A.rows(); ++i) {
        for (Index j = 0; j < A.cols(); ++j) {
          sum += A(i, j);
        }
      }
    }
  )
  cout << "test_at_vs_operator_paren: " << sum << endl;
}

void test_default_init(int cnt)
{
  int x = 0;
  BENCH("m:default_init:constructor", "test if matrix is allocated but not initialized: use constructor Mat(Index rows, Index cols)", cnt,
    for (int i = 0; i < cnt; ++i) {
      Matrix<int> A(100, 100);
    }
  )
  cout << "Matrix(row, col) constructor:" << x << endl;

  BENCH("m:default_init:vector", "test if matrix is allocated but not initialized: use vector(size_t n)", cnt,
    for (int i = 0; i < cnt; ++i) {
      vector<int> B(10000);
      x = B[0];
    }
  )
  if (x > 0) {
    return;
  }
}

void test_unary_negate(int cnt)
{
}

int main(int argc, char const *argv[])
{
  int* p = new int; // NOLINT
  cout << *p << endl; // NOLINT
  p = new int[10000000]; // NOLINT
  cout << p[10000000 - 2] << endl; // NOLINT
  
  cout << traits::scalar_traits<int>::one << endl;
  cout << traits::scalar_traits<float>::one << endl;
  cout << traits::scalar_traits<const int&>::one << endl;

  Matrix<int> A(3, 3);
  A = {1, 2, 1, 4, 5, 4, 1, -1, 0};
  Matrix<int> B(3, 3);
  B = -A;
  B = A;

  Matrix<int> C(3, 3);
  mul::multiplyMatrix<Index>(A, B, C);
  cout << "multiplyMatrix:\n" << C << endl;
  C = {
    0, 1, 0,
    0, 0, 1,
    0, 0, 0,
  };

  cout << "A = \n" << A << endl;
  C.MulLeftTo(A);
  C.MulRightTo(A);
  cout << A << endl;
  cout << C.transpose() << endl;
  cout << A << endl;
  cout << C << endl;
  cout << A * C << endl;

  test_add_eq_mul(A, 10);
  test_sub_eq_mul(A, 10);
  test_at_vs_operator_paren(B, 1000 * 1000);
  test_default_init(1000);

  cout << A(0, 0) << endl;
  cout << A(0, 2) << endl;
  cout << "hello" << endl;
  cout << A.size() << endl;
  cout << A.rows() << endl;
  cout << B(0, 0) << endl;
  B(1, 2) = 4;
  cout << B[5] << endl;

  auto D = Matrix<double>::eye(3, 5);
  cout << "eye:\n" << D << endl;
  cout << D * 3.0 << endl;

  constexpr auto v = Matrix<int, 3, 3>({1, 2, 3})[2];
  // constexpr auto vv = Matrix<int>({1, 2, 3}); // should error
  constexpr auto w = Matrix<int, 3, 3>({1, 2, 3}).at(0, 2);
  constexpr auto E = Matrix<double, 3, v>({1, 2});
  cout << E << endl;
  cout << w << endl;

  constexpr auto F = Matrix<double, 3, 3>({0, 0, 1});
  constexpr auto G = E * F;
  return 0;
}