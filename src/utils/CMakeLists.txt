set(utils_SRCS
  Timestamp.cc
)

add_library(wethands_utils OBJECT ${utils_SRCS})

add_subdirectory(tests)
