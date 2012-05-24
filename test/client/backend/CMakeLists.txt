include_directories(
    ${CMAKE_BINARY_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/client/backend
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QCA2_INCLUDE_DIR}
)

# ################################
# SessionManager test
# ################################

set(WORKING_DIR ${CMAKE_SOURCE_DIR}/test)
set(CA_CERT ${CMAKE_SOURCE_DIR}/test/ca.crt)
set(SERVER_URL "selinux.inso.tuwien.ac.at")
unset(RUN_SERVER)
configure_file(
    ${CMAKE_SOURCE_DIR}/test/common.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/common.h
)

set(sessionManagerTest_QTHEADERS
    sessionmanagertest.h
)

qt4_wrap_cpp(sessionManagerTest_QTHEADERS_MOC
    ${sessionManagerTest_QTHEADERS}
)

add_executable(sessionManagerTest
    sessionmanagertest.cpp
    ${sessionManagerTest_QTHEADERS_MOC}
)

add_dependencies(sessionManagerTest
    client_backend
)

target_link_libraries(sessionManagerTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_dependencies(sessionManagerTest
    client_backend
)

add_test(NAME sessionManagerTest
    COMMAND sessionManagerTest
)

# ################################
# Session test
# ################################

set(sessionTest_QTHEADERS
    sessiontest.h
)

qt4_wrap_cpp(sessionTest_QTHEADERS_MOC
    ${sessionTest_QTHEADERS}
)

add_executable(sessionTest
    sessiontest.cpp
    ${sessionTest_QTHEADERS_MOC}
)

add_dependencies(sessionTest
    client_backend
)

target_link_libraries(sessionTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_dependencies(sessionTest
    client_backend
)

add_test(NAME sessionTest
    COMMAND sessionTest
)

# ################################
# Chat test
# ################################

set(chatTest_QTHEADERS
    chattest.h
)

qt4_wrap_cpp(chatTest_QTHEADERS_MOC
    ${chatTest_QTHEADERS}
)

add_executable(chatTest
    chattest.cpp
    ${chatTest_QTHEADERS_MOC}
)

add_dependencies(chatTest
    client_backend
)

target_link_libraries(chatTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
    ${QCA2_LIBRARIES}
)

add_dependencies(chatTest
    client_backend
)

add_test(NAME chatTest
    COMMAND chatTest

)
