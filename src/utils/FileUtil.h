// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-19 23:08:59
// Description:

#ifndef SRC_UTILS_FILEUTIL_H_
#define SRC_UTILS_FILEUTIL_H_

#include <unistd.h>
#include <string>
#include "src/utils/Uncopyable.h"

namespace wethands {
namespace FileUtil {

// 文件的封装类, 追加方式写. 非线程安全的.
class AppendFile : public Uncopyable {
 public:
  explicit AppendFile(const std::string& filename);
  explicit AppendFile(const char* filename);
  ~AppendFile();

  // 追加写. 内部使用::fwrite_unlocked.
  void Append(const char* line, size_t len);
  void Flush();
  off_t WrittenBytes() const { return writtenBytes_; }

 private:
  FILE* fp_;
  off_t writtenBytes_;
  char buffer_[64*1024];  // 64KB缓冲区. 为了减少系统调用次数.
};

}  // namespace FileUtil
}  // namespace wethands

#endif  // SRC_UTILS_FILEUTIL_H_
