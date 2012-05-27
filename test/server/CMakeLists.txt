include_directories(
    ${CMAKE_BINARY_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QCA2_INCLUDE_DIR}
    ${QT_QTSQL_INCLUDE_DIR}
)

set(WORKING_DIR ${CMAKE_SOURCE_DIR}/test)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(BINARY_DIR ${CMAKE_BINARY_DIR})
configure_file(
    ${CMAKE_SOURCE_DIR}/test/common.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/common.h
)
configure_file(
    ${CMAKE_SOURCE_DIR}/test/sdcs.conf.in
    ${CMAKE_BINARY_DIR}/test/sdcs.conf
)

set(userDbProxyTest_QTHEADERS
    userdbproxytest.h
)

qt4_wrap_cpp(userDbProxyTest_QTHEADERS_MOC
    ${userDbProxyTest_QTHEADERS}
)

add_executable(userDbProxyTest
    userdbproxytest.cpp
    ${userDbProxyTest_QTHEADERS_MOC}
)

add_dependencies(userDbProxyTest
    server
)

target_link_libraries(userDbProxyTest
    Shared
    serverlib
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_test(NAME userDbProxyTest
    COMMAND userDbProxyTest
)
