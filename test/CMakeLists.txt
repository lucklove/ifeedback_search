SET(TEST_SOURCES
	test.cc
    cppjieba.cc
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/3rdparty/inc
)

ADD_EXECUTABLE(ifeedback_search_test ${TEST_SOURCES})

ADD_CUSTOM_TARGET(run_test COMMAND ${CMAKE_BINARY_DIR}/test/ifeedback_search_test DEPENDS ifeedback_search_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
