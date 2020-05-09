// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-09 18:33:46
// Description: 简单代替boost/operators.hpp

#ifndef SRC_UTILS_OPERATORS_H_
#define SRC_UTILS_OPERATORS_H_

namespace wethands {

// 指明一个类可以用运算符 < 比较.
// 使用方法是继承它, 并实现操作符 operator<, 它会自动实现 >, <=, >=.
// 没有考虑多继承的问题, 使用时注意.
template <typename T>
struct LessThanComparable {
  friend bool operator>(const T& x, const T& y) { return y < x; }

  friend bool operator<=(const T& x, const T& y) {
    return !static_cast<bool>(y < x);
  }

  friend bool operator>=(const T& x, const T& y) {
    return !static_cast<bool>(x < y);
  }
};

// 指明一个类可以用运算符 == 比较.
// 使用方法是继承它, 并实现操作符 operator==, 它会自动实现 !=.
// 没有考虑多继承的问题, 使用时注意.
template <typename T>
struct EqualityComparable {
  friend bool operator!=(const T& x, const T& y) {
    return !static_cast<bool>(x == y);
  }
};

}  // namespace wethands

#endif  // SRC_UTILS_OPERATORS_H_
