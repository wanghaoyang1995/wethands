// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-09 12:51:42
// Description:

#ifndef SRC_UTILS_UNCOPYABLE_H_
#define SRC_UTILS_UNCOPYABLE_H_

namespace wethands {

// 标记一个类是不可拷贝的(对象语义). 需要时继承它.
class Uncopyable {
 protected:
  Uncopyable() = default;
  Uncopyable(const Uncopyable&) = delete;
  Uncopyable& operator=(const Uncopyable&) = delete;
  ~Uncopyable() = default;
};

}  // namespace wethands

#endif  // SRC_UTILS_UNCOPYABLE_H_
