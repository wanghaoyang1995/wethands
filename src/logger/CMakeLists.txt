set(logger_SRCS
  Logger.cc
)

add_library(wethands_logger OBJECT ${logger_SRCS})

add_subdirectory(tests)
