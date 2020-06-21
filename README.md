# wethands
## 简介
仿muduo实现的高性能C++网络库

muduo是一个基于Reactor模式的多线程C++网络库，作者[陈硕](https://me.csdn.net/solstice)于2010年完成。
muduo项目地址：https://github.com/chenshuo/muduo

出于学习目的，wethands 仿照 muduo 库代码，实现了一个更轻量的C++网络库。

## 编译运行
```bash
# debug
cd wethands/
./build.sh

# release
cd wethands/
BUILD_TYPE=release ./build.sh
```

## 编译运行需求
linux 2.6 +  
gcc 4.7 +  
cmake 3.10.2 +  

## 了解更多
https://blog.csdn.net/sinat_27953939/article/details/106866049
