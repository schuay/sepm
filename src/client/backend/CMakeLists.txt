include_directories(
    ${CMAKE_BINARY_DIR}/src/shared/
    ${CMAKE_SOURCE_DIR}/src/shared/
    ${QCA2_INCLUDE_DIR}
)

set(client_backend_SRCS
    chat.cpp
    session.cpp
    sessionmanager.cpp
    user.cpp
    loginuser.cpp
)

set(client_backend_QTHEADERS
    chat.h
    session.h
    sessionmanager.h
)

qt4_wrap_cpp(client_backend_QTHEADERS_MOC
    ${client_backend_QTHEADERS}
)

add_library(client_backend STATIC
    ${client_backend_SRCS}
    ${client_backend_QTHEADERS_MOC}
)

target_link_libraries(client_backend
    ${QT_LIBRARIES}
    ${ZeroCIce_LIBRARIES}
)

add_dependencies(client_backend
    Shared
)
