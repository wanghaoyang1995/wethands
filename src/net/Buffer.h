// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-02 23:13:25
// Description:

#ifndef SRC_NET_BUFFER_H_
#define SRC_NET_BUFFER_H_

#include <unistd.h>
#include <string>
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

  // 取出 len 可读字节. 读指针向后移动 len.
  void Retrieve(size_t len);
  // 取出 end 之前的字节, 读指针后移至end.
  void RetrieveUntil(const char* end);
  // 取出所有可读字节. 读写指针归复位.
  void RetrieveAll();
  // 取出 len 字节, 以 std::string 返回.
  std::string RetrieveAsString(size_t len);
  // 取出所有字节, 以 std::string 返回.
  std::string RetrieveAllAsString();
  // 写指针向后移动 len.
  void HasWriten(size_t len);
  // 写指针向前移动 len.
  void Unwrite(size_t len);
  // 返回缓冲区可读指针.
  const char* Peek() const { return buffer_.data() + readIndex_; }
  // 返回缓冲区的首地址指针.
  const char* Begin() const { return buffer_.data(); }
  char* Begin() { return buffer_.data(); }
  // 返回缓冲区可写指针.
  const char* BeginWrite() const { return buffer_.data() + writeIndex_; }
  char* BeginWrite() { return buffer_.data() + writeIndex_; }
  // 前置 len 字节. 并将读指针前移 len.
  void Prepend(const void* data, size_t len);
  // 追加 len 字节.
  void Append(const void* data, size_t len);
  void Append(const std::string& data);
  // 从 fd 中读数据到缓冲区.
  ssize_t ReadFd(int fd, int* errorCode);
  const char* FindCRLF() const;

 private:
  // 检查是否可写入 len 字节, 若否则整理空间以容纳 len 字节可写.
  void CheckAndMakePlace(size_t len);

  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;

  static constexpr size_t kInitialSize = 1024;  // 初始可写大小 1KiB.
  static constexpr size_t kInitPrependSize = 8;
  static const char kCRLF[];
};

}  // namespace wethands

#endif  // SRC_NET_BUFFER_H_
