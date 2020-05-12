add_subdirectory(utils)
add_subdirectory(thread)
add_subdirectory(logger)

add_library(wethands $<TARGET_OBJECTS:wethands_utils>
                     $<TARGET_OBJECTS:wethands_thread>
                     $<TARGET_OBJECTS:wethands_logger>)
target_link_libraries(wethands pthread rt)
