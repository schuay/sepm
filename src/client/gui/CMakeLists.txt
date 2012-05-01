include_directories(
        ${CMAKE_SOURCE_DIR}/src/client/backend/
        ${CMAKE_SOURCE_DIR}/src/shared/
        ${CMAKE_BINARY_DIR}/src/shared/
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QCA2_INCLUDE_DIR}
)

set(client_gui_FORMS
        logindialog.ui
)
qt4_wrap_ui(client_gui_FORMS_HEADERS ${client_gui_FORMS})

set(client_gui_QTHEADERS
        logindialog.h
)
qt4_wrap_cpp(client_gui_QTHEADERS_MOC ${client_gui_QTHEADERS})

set(client_gui_SRCS
        logindialog.cpp
)

add_library(client_gui STATIC
        ${client_gui_SRCS}
        ${client_gui_QTHEADERS_MOC}
        ${client_gui_FORMS_HEADERS}
)

target_link_libraries(client_gui
        client_backend
        Shared
        ${QT_LIBRARIES}
)
