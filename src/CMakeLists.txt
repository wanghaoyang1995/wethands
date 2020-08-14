add_subdirectory(utils)
add_subdirectory(thread)
add_subdirectory(logger)
add_subdirectory(reactor)
add_subdirectory(net)

add_library(wethands $<TARGET_OBJECTS:wethands_utils>
                     $<TARGET_OBJECTS:wethands_thread>
                     $<TARGET_OBJECTS:wethands_logger>
                     $<TARGET_OBJECTS:wethands_reactor>
                     $<TARGET_OBJECTS:wethands_net>
                     $<TARGET_OBJECTS:wethands_http>)
target_link_libraries(wethands pthread rt)
