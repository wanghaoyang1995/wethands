set(logger_SRCS
  AsyncLogging.cc
  LogFile.cc
  Logger.cc
)

add_library(wethands_logger OBJECT ${logger_SRCS})

add_subdirectory(tests)
