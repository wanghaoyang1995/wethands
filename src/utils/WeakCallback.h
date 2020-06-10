// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-10 21:49:27
// Description:

#ifndef SRC_UTILS_WEAKCALLBACK_H_
#define SRC_UTILS_WEAKCALLBACK_H_

#include <memory>

namespace wethands {
// 弱回调函数, 与普通仿函类似.
// 区别在于该类会使用 weak_ptr 判断对象是否存活, 若是才调用.
template <typename T, typename... ARGS>
class WeakCallback {
 public:
  WeakCallback(const std::weak_ptr<T>& obj,
               const std::function<void (T*, ARGS...)>& functor)
      : obj_(obj), functor_(functor) {}
  ~WeakCallback() = default;

  void operator()(ARGS&&... args) const {
    std::shared_ptr<T> ptr(obj_.lock());
    if (ptr) {
      functor_(ptr.get(), std::forward<ARGS>(args)...);
    }
  }

 private:
  std::weak_ptr<T> obj_;  // 要检查的对象.
  std::function<void (T*, ARGS...)> functor_;
};

template <typename T, typename... ARGS>
WeakCallback<T, ARGS...> MakeWeakCallback(const std::shared_ptr<T>& obj,
                                          void (T::*funcPtr)(ARGS...)) {
  return WeakCallback<T, ARGS...>(obj, funcPtr);
}

template <typename T, typename... ARGS>
WeakCallback<T, ARGS...> MakeWeakCallback(const std::shared_ptr<T>& obj,
                                          void (T::*funcPtr)(ARGS...) const) {
  return WeakCallback<T, ARGS...>(obj, funcPtr);
}

}  // namespace wethands

#endif  // SRC_UTILS_WEAKCALLBACK_H_
