// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 23:56:29
// Description:

#ifndef SRC_THREAD_THREAD_H_
#define SRC_THREAD_THREAD_H_

#include <functional>
#include "src/utils/Uncopyable.h"

namespace wethands {
// pthread线程的封装类. 不可拷贝.
class Thread : public Uncopyable {
 public:
  typedef std::function<void()> ThreadFunc;

  explicit Thread(ThreadFunc func);

 private:
  ThreadFunc func_;
  bool joined;

};

}  // namespace wethands

#endif  // SRC_THREAD_THREAD_H_