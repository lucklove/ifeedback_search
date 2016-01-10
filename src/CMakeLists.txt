SET(IFEEDBACK_SEARCH_SRCS
    main.cc
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/3rdparty/inc
    /usr/include/mysql
)

LINK_DIRECTORIES(/usr/local/lib64)
ADD_EXECUTABLE(ifeedback_search ${IFEEDBACK_SEARCH_SRCS})
TARGET_LINK_LIBRARIES(ifeedback_search boost_system pthread soci_core soci_mysql)
