add_subdirectory(utils)
add_subdirectory(thread)

add_library(wethands $<TARGET_OBJECTS:wethands_utils>
                     $<TARGET_OBJECTS:wethands_thread>)
target_link_libraries(wethands pthread rt)
