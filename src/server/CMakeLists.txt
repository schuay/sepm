include_directories(
        ${CMAKE_SOURCE_DIR}/src/shared/
        ${CMAKE_BINARY_DIR}/src/shared/
        ${QCA2_INCLUDE_DIR}
        ${QT_QTSQL_INCLUDE_DIR}
)

add_library(serverlib STATIC
        authentication.cpp
        chat.cpp
        interserverfactory.cpp
        participant.cpp
        session.cpp
        userdbproxy.cpp
)

target_link_libraries(serverlib
        Shared
        ${ZeroCIce_LIBRARIES}
        ${QT_LIBRARIES}
        ${QT_QTTEST_LIBRARY}
        ${QCA2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(server
        main.cpp
)

target_link_libraries(server
        serverlib
        Shared
        ${ZeroCIce_LIBRARIES}
        ${QT_LIBRARIES}
        ${QT_QTTEST_LIBRARY}
        ${QCA2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)
