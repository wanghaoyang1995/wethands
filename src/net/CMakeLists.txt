set(net_SRCS
  Acceptor.cc
  Buffer.cc
  Connector.cc
  InetAddress.cc
  Socket.cc
  TcpConnection.cc
  TcpClient.cc
  TcpServer.cc
)

add_library(wethands_net OBJECT ${net_SRCS})

add_subdirectory(tests)
