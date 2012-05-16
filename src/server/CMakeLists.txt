include_directories(
        ${CMAKE_SOURCE_DIR}/src/shared/
        ${CMAKE_BINARY_DIR}/src/shared/
        ${QCA2_INCLUDE_DIR}
)

add_executable(server
        main.cpp
        authentication.cpp
        chat.cpp
        interserverfactory.cpp
        participant.cpp
        session.cpp
        userdbproxy.cpp
)

target_link_libraries(server
        ${QT_LIBRARIES}
        ${QCA2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)
