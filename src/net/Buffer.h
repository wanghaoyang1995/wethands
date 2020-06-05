// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-02 23:13:25
// Description:

#ifndef SRC_NET_BUFFER_H_
#define SRC_NET_BUFFER_H_

#include <vector>
#include "src/utils/Copyable.h"

namespace wethands {
// 用于读写的缓冲区.
// 可拷贝的.
class Buffer : public Copyable {
 public:
  explicit Buffer(size_t initialSize);
  ~Buffer() = default;

  void Swap(Buffer& rhs);

  size_t ReadableBytes() const;
  size_t WritableBytes() const;
  // 返回前置位置可存放的字节数.
  size_t PrependableBytes() const;



 private:
  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;
};

}  // namespace wethands

#endif  // SRC_NET_BUFFER_H_
