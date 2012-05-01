add_subdirectory(backend)
add_subdirectory(gui)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/backend
        ${CMAKE_CURRENT_SOURCE_DIR}/gui
        ${CMAKE_SOURCE_DIR}/src/shared/
)

add_executable(client
        main.cpp
)

target_link_libraries(client
        client_backend
        client_gui
        ${QT_LIBRARIES}
        ${QCA2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
)
