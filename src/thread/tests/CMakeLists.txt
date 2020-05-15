add_executable(CurrentThread_test CurrentThread_test.cc)
target_link_libraries(CurrentThread_test wethands)

add_executable(Atomic_test Atomic_test.cc)
target_link_libraries(Atomic_test wethands)

add_executable(Mutex_test Mutex_test.cc)
target_link_libraries(Mutex_test wethands)

add_executable(Condition_test Condition_test.cc)
target_link_libraries(Condition_test wethands)

add_executable(CountDownLatch_test CountDownLatch_test.cc)
target_link_libraries(CountDownLatch_test wethands)

add_executable(Thread_test Thread_test.cc)
target_link_libraries(Thread_test wethands)

add_executable(ThreadPool_test ThreadPool_test.cc)
target_link_libraries(ThreadPool_test wethands)
