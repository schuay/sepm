include_directories(
    ${CMAKE_BINARY_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/client/backend
)

set(backendTest_QTHEADERS
    test.h
)

qt4_wrap_cpp(backendTest_QTHEADERS_MOC
    ${backendTest_QTHEADERS}
)

add_executable(backendTest
    test.cpp
    ${backendTest_QTHEADERS_MOC}
)

add_dependencies(backendTest
    client_backend
)

target_link_libraries(backendTest
    client_backend
    Shared
    ${QT_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZeroCIce_LIBRARIES}
)

add_dependencies(backendTest
    client_backend
)
