// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-19 23:15:20
// Description:

#ifndef SRC_UTILS_SINGLETON_H_
#define SRC_UTILS_SINGLETON_H_

#include <pthread.h>
#include <cassert>
#include <cstdlib>
#include "src/utils/Uncopyable.h"

namespace wethands {
// 单例类模版.
// 模板参数类型必须有无参的构造函数.
template <typename T>
class Singleton : public Uncopyable {
 public:
  Singleton() = delete;
  ~Singleton() = delete;

  static T& GetInstance() {
    ::pthread_once(&initFlag_, Init);
    return *ptr_;
  }

 private:
  static void Init() {
    ptr_ = new T();
    ::atexit(Destory);
  }

  static void Destory() {
    delete ptr_;
    ptr_ = nullptr;
  }

  static T* ptr_;
  static pthread_once_t initFlag_;
};

template <typename T>
T* Singleton<T>::ptr_ = nullptr;

template <typename T>
pthread_once_t Singleton<T>::initFlag_ = PTHREAD_ONCE_INIT;

}  // namespace wethands

#endif  // SRC_UTILS_SINGLETON_H_
