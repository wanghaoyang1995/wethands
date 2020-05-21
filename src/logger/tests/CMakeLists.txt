add_executable(Logger_test Logger_test.cc)
target_link_libraries(Logger_test wethands)

add_executable(LogFile_test LogFile_test.cc)
target_link_libraries(LogFile_test wethands)

add_executable(AsyncLogging_test AsyncLogging_test.cc)
target_link_libraries(AsyncLogging_test wethands)
