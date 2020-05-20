set(utils_SRCS
  Timestamp.cc
  FileUtil.cc
)

add_library(wethands_utils OBJECT ${utils_SRCS})

add_subdirectory(tests)
