// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-02 23:13:25
// Description:

#ifndef SRC_NET_BUFFER_H_
#define SRC_NET_BUFFER_H_

#include <vector>
#include "src/utils/Copyable.h"

namespace wethands {
// 用于读写的可增长缓冲区.
// 可拷贝的.
class Buffer : public Copyable {
 public:
  explicit Buffer(size_t initialSize = kInitialSize);
  ~Buffer() = default;

  void Swap(Buffer& rhs);

  size_t ReadableBytes() const { return writeIndex_ - readIndex_; }
  size_t WritableBytes() const { return buffer_.size() - writeIndex_; }
  // 返回前置位置可存放的字节数.
  size_t PrependableBytes() const { return readIndex_; }

  // 取出 len 字节.
  void Retrieve(size_t len);
  const char* Peek() const { return buffer_.data() + readIndex_; }
  void Prepend(const char* data, size_t len);
  void Append(const char* data, size_t len);



 private:
  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;

  static constexpr size_t kInitialSize = 1024;
  static constexpr size_t kPrependSize = 8;
};

}  // namespace wethands

#endif  // SRC_NET_BUFFER_H_
