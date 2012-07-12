ADD_EXECUTABLE(parallel_pow_test parallel_pow_test.cpp parallel_pow.hpp)
TARGET_LINK_LIBRARIES(parallel_pow_test ${TBB_LIBRARIES})

ADD_TEST(parallel_pow_test parallel_pow_test)
