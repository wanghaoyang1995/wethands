add_subdirectory(utils)

add_library(wethands $<TARGET_OBJECTS:wethands_utils>)
target_link_libraries(wethands pthread rt)
