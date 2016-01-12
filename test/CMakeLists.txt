SET(TEST_SOURCES
	test.cc
    cppjieba.cc
    search.cc
    connection_pool.cc
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/3rdparty/inc
    /usr/include/mysql
)

LINK_DIRECTORIES(/usr/local/lib64)
ADD_EXECUTABLE(ifeedback_search_test ${TEST_SOURCES})
TARGET_LINK_LIBRARIES(ifeedback_search_test soci_core soci_mysql atomic)

ADD_CUSTOM_TARGET(run_test COMMAND ${CMAKE_BINARY_DIR}/test/ifeedback_search_test DEPENDS ifeedback_search_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
