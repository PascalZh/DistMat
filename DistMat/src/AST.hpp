#pragma once
#include <string>
#include <variant>

struct Op {
  std::string_view name;
};

struct Var {
  std::string_view name;
};

constexpr std::string_view ADD = "+";
constexpr std::string_view SUB = "-";
constexpr std::string_view MUL = "*";
constexpr std::string_view DIV = "/"; 

struct BinOp;
struct CoeffWiseOp;

using Expr = std::variant<Var, BinOp, CoeffWiseOp>;

struct BinOp : Op {
  Expr* lhs;
  Expr* rhs;
};

struct CoeffWiseOp : Op {
  Expr* src;
};

constexpr auto parseExpr(const Expr& expr) {

}

void eval(const Expr& expr)
{
  parseExpr(expr);
}