set(net_SRCS
  Acceptor.cc
  Connector.cc
  InetAddress.cc
  Socket.cc
)

add_library(wethands_net OBJECT ${net_SRCS})

add_subdirectory(tests)
