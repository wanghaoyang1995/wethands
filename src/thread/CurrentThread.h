// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-11 22:59:51
// Description:

#ifndef SRC_THREAD_CURRENTTHREAD_H_

#include <cstdint>

namespace wethands {
// 命名空间中包含了当前线程的一些信息.
namespace CurrentThread {

// 缓存了当前线程id, 线程局部数据.
// 目的是为了尽量减少系统调用次数.
extern __thread int t_tid;

// 获取当前线程tid.
// 如果没有缓存过, 就调用系统调用将其缓存. 否则返回缓存的值.
int Tid();

// 如果当前线程id等于进程id, 返回true.
bool IsMainThread();

// 使当前线程休眠usec微秒的时间.
void sleepUsec(int64_t usec);

}  // namespace CurrentThread
}  // namespace wethands

#define SRC_THREAD_CURRENTTHREAD_H_
#endif  // SRC_THREAD_CURRENTTHREAD_H_
