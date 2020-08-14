set(http_SRCS
  HttpRequest.cc
)

add_library(wethands_http OBJECT ${http_SRCS})

add_subdirectory(tests)
