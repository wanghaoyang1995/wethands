// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-09 12:51:20
// Description:

#ifndef SRC_UTILS_COPYABLE_H_
#define SRC_UTILS_COPYABLE_H_

namespace wethands {

// 标记一个类是可拷贝的(值语义). 需要时继承它.
class Copyable {
 protected:
  Copyable() = default;
  ~Copyable() = default;
};

}  // namespace wethands

#endif  // SRC_UTILS_COPYABLE_H_
