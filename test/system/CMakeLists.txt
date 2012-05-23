include_directories(
    ${CMAKE_BINARY_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/client/backend
    ${CMAKE_SOURCE_DIR}/test/client/backend
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QCA2_INCLUDE_DIR}
    ${QT_QTSQL_INCLUDE_DIR}
)

# ################################
# SessionManager test (local server)
# ################################

set(TEST_SOURCE ${CMAKE_SOURCE_DIR}/test/client/backend)
set(WORKING_DIR ${CMAKE_SOURCE_DIR}/test)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(BINARY_DIR ${CMAKE_BINARY_DIR})
set(SERVER_URL "localhost")
set(RUN_SERVER ON)
configure_file(
    ${CMAKE_SOURCE_DIR}/test/common.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/common.h
)

set(localSessionManagerTest_QTHEADERS
    ${TEST_SOURCE}/sessionmanagertest.h
)

qt4_wrap_cpp(localSessionManagerTest_QTHEADERS_MOC
    ${localSessionManagerTest_QTHEADERS}
)

add_executable(localSessionManagerTest
    ${TEST_SOURCE}/sessionmanagertest.cpp
    ${localSessionManagerTest_QTHEADERS_MOC}
)

add_dependencies(localSessionManagerTest
    client_backend
)

target_link_libraries(localSessionManagerTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_dependencies(localSessionManagerTest
    client_backend
)

add_test(NAME localSessionManagerTest
    COMMAND localSessionManagerTest
)

# ################################
# Session test
# ################################

set(localSessionTest_QTHEADERS
    ${TEST_SOURCE}/sessiontest.h
)

qt4_wrap_cpp(localSessionTest_QTHEADERS_MOC
    ${localSessionTest_QTHEADERS}
)

add_executable(localSessionTest
    ${TEST_SOURCE}/sessiontest.cpp
    ${localSessionTest_QTHEADERS_MOC}
)

add_dependencies(localSessionTest
    client_backend
)

target_link_libraries(localSessionTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_dependencies(localSessionTest
    client_backend
)

add_test(NAME localSessionTest
    COMMAND localSessionTest
)

# ################################
# Chat test
# ################################

set(localChatTest_QTHEADERS
    ${TEST_SOURCE}/chattest.h
)

qt4_wrap_cpp(localChatTest_QTHEADERS_MOC
    ${localChatTest_QTHEADERS}
)

add_executable(localChatTest
    ${TEST_SOURCE}/chattest.cpp
    ${localChatTest_QTHEADERS_MOC}
)

add_dependencies(localChatTest
    client_backend
)

target_link_libraries(localChatTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_dependencies(localChatTest
    client_backend
)

add_test(NAME localChatTest
    COMMAND localChatTest
)
