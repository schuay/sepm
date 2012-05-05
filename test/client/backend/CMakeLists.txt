include_directories(
    ${CMAKE_BINARY_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/client/backend
    ${QCA2_INCLUDE_DIR}
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
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test/client/backend
)

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
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test/client/backend
)
