ADD_EXECUTABLE(parallel_histogram_test parallel_histogram_test.cpp parallel_histogram.hpp)
TARGET_LINK_LIBRARIES(parallel_histogram_test ${TBB_LIBRARIES})

ADD_TEST(parallel_histogram_test parallel_histogram_test)
