# DistMat
A distributed matrix library for easy use.

# class `Matrix`
`Matrix` now have first 3 template parameters fixed, may not change in the future:
```cpp
template<typename Scalar, int Rows, int Cols>
class Matrix;
```
Other parameters are likely to be changed in the future evolution.

## Default template parameters
Default template parameters is provided in forward declaration, so in the definition of `Matrix`, full parameters list are required.

## Heap vs. Stack
Two version of `Matrix` is provided:
- in the case of `Rows > 0 && Cols > 0`, use stack version of Matrix
- in the case of `Rows == -1 && Cols == -1`, use heap version of Matrix
- otherwise, error

For some member functions/member variables, two versions is provided, differed by concept or SFINAE. Here are some examples:
```cpp
// make a non-template function to a template
// different conditions are provided in various form
template<typename T = Scalar>
  requires is_same_v<T, Scalar> && (Rows == -1) && (Cols == -1)  // here, conditions for heap version
Matrix(Index rows, Index cols)
  : m_storage(rows * cols), m_shape{rows, cols} {}

template<typename T = Scalar>
  requires is_same_v<T, Scalar>
constexpr explicit Matrix(InternalStorage storage)
  : m_storage{std::move(storage)}
{
  if constexpr(!(Rows > 0 && Cols > 0)) {  // here, conditions for stack version
    static_assert(!is_same_v<T, T>, "Can not initialize the matrix without shape specified!");
  }
}
```

Other common functions are shared.

### Stack version
Stack version of `Matrix` is provided to be `constexpr`, matrix plus/minus/multiplication are capable of being evaluated at compile time.

# Naming conventions
1. function and variable: `fooBar`
2. private member variable: `foo_`, `fooBar_`
3. class name: `MyClass`, `foo_bar`(for template meta programming)
4. special case `Is_rows_Implemented`: `rows` is preserved lowercase and separated by `_` because `rows` represents a function named `rows`.

# Compile time parsing matrix expression for optimizing

## Feasibility experiments

### 2021/6/19: try optimize `x = x + 2` to `x += 2`

**details**:
In `experiment_expr.cpp`, `x = x + 2` does compile to `add eax, 2`, but it relies `-O2` compiler flag.

**conclusion**:
It's impossible for `operator=` to figure out whether `*this` is the same as the `x` in `x + 2` at compile time on language level. But the optimization does happen on some compilers and with some specific compiler flags.

So, maybe we can write some optimization at run-time and rely on the compiler to optimize out unnecessary codes like in `experiment_expr.cpp`?

### 2021/6/20: learn Eigen

When evaluating expressions that `x + x`, Eigen return a expression with the type representing the expression and two references as members (`m_lhs` and `m_rhs`). When evaluating `operator=`, Eigen will choose to do no alias assignment or alias assignment depending on the expression. e.g. `x = x + x` expands to expressions like `x[i] = x[i] + x[i]`, while `x = x * x` expands to `Matrix tmp(x * x); x = tmp`.