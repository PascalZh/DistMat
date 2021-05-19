#pragma once
#include <stdexcept>
#include <string>
#include <iostream>
#include "Type.h"

namespace DistMat
{
namespace error
{
#define ERROR_WHERE() string("\n\t") + __FILE__ + ":" + to_string(__LINE__) + "\n\tFunction: " + __func__

#define CHECK_DIM(lhs, rhs) \
  if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols()) {\
    throw std::runtime_error(ERROR_WHERE() + "\n\tError: " +\
    "shape (" + to_string(lhs.rows()) + ", " + to_string(lhs.cols()) +\
    ") doesn't match with shape (" + to_string(rhs.rows()) +\
    ", " + to_string(rhs.cols()) + ")");\
  }

} // namespace error
} // namespace DistMat
