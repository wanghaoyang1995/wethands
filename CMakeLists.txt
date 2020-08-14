cmake_minimum_required(VERSION 3.10.2)

project(wethands)

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
set(CXX_FLAGS
  -D_FILE_OFFSET_BITS=64 # 使得32位Linux能够处理大于2GB的文件
  -Wall                  # 打开大部分警告
  -Wextra                # 打开额外警告信息
  -Werror                # 将警告视为错误
  -Wold-style-cast       # 使用了旧式类型转换时发出警告
  #-Wno-unused-parameter # 当函数有未使用的保留参数时，抑制警告
  -Woverloaded-virtual   # 隐藏了虚函数时发出警告
  -Wpointer-arith        # 对指针进行算术运算时发出警告
  -Wshadow               # 名字隐藏时发出警告
  -Wwrite-strings        # 字符串常量赋值给char*时发出警告
  -march=native          # 启用本地处理器所有的指令集
  -std=c++11
  #-rdynamic             # 在程序内部获取调用栈时需要开启该链接选项。https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html
)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(src wethands)

# GTEST
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
