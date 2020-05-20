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

// 只写文件的封装类. 以追加方式写. 非线程安全的.
class AppendFile : public Uncopyable {
 public:
  // 参数filename是相对路径或绝对路径.
  explicit AppendFile(const std::string& filename);
  explicit AppendFile(const char* filename);
  ~AppendFile();

  // 追加写len字节line中的数据. 内部使用::fwrite_unlocked.
  // 写够len字节返回, 或者出错返回.
  void Append(const char* line, size_t len);
  void Flush();
  off_t WrittenBytes() const { return writtenBytes_; }

 private:
  FILE* fp_;
  off_t writtenBytes_;  // 已写字节数.
  char buffer_[64*1024];  // 自定义缓冲区大小. 给fwrite_unlocked使用.
};

}  // namespace FileUtil
}  // namespace wethands

#endif  // SRC_UTILS_FILEUTIL_H_
