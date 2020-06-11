add_executable(Acceptor_test Acceptor_test.cc)
target_link_libraries(Acceptor_test wethands)

add_executable(Connector_test Connector_test.cc)
target_link_libraries(Connector_test wethands)

add_executable(InetAddress_test InetAddress_test.cc)
target_link_libraries(InetAddress_test wethands)

add_executable(EchoServer EchoServer.cc)
target_link_libraries(EchoServer wethands)

add_executable(EchoClient EchoClient.cc)
target_link_libraries(EchoClient wethands)
