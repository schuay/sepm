include_directories(
        ${CMAKE_SOURCE_DIR}/src/client/backend/
        ${CMAKE_SOURCE_DIR}/src/shared/
        ${CMAKE_BINARY_DIR}/src/shared/
        ${QCA2_INCLUDE_DIR}
)
set(client_gui_SRCS
        loginwindow.cpp
)
set(client_gui_QTHEADERS
        loginwindow.h
)

set(client_gui_QTRESOURCES
        "res/client_gui_qml.qrc"
)

qt4_add_resources(client_gui_QTRESOURCES_RCC
        ${client_gui_QTRESOURCES}
)

qt4_wrap_cpp(
        client_gui_QTHEADERS_MOC
        ${client_gui_QTHEADERS}
)

add_library(client_gui STATIC
        ${client_gui_SRCS}
        ${client_gui_QTHEADERS_MOC}
        ${client_gui_QTRESOURCES_RCC}
)

target_link_libraries(client_gui
        client_backend
        Shared
        ${QT_LIBRARIES}
)
