set(http_SRCS
  HttpContext.cc
  HttpRequest.cc
  HttpResponse.cc
  HttpServer.cc
)

add_library(wethands_http OBJECT ${http_SRCS})

add_subdirectory(tests)
