include_directories(
        ${CMAKE_SOURCE_DIR}/src/client/backend/
        ${CMAKE_SOURCE_DIR}/src/shared/
        ${CMAKE_BINARY_DIR}/src/shared/
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${QCA2_INCLUDE_DIR}
)

set(client_gui_FORMS
        logindialog.ui
        appwindow.ui
        chatwidget.ui
        chatlogdialog.ui
        settingsdialog.ui
)
qt4_wrap_ui(client_gui_FORMS_HEADERS ${client_gui_FORMS})

set(client_gui_QTHEADERS
        logindialog.h
        appwindow.h
        chatwidget.h
        chatlogdialog.h
        settingsdialog.h
        usermodel.h
)
qt4_wrap_cpp(client_gui_QTHEADERS_MOC ${client_gui_QTHEADERS})

set(client_gui_SRCS
        logindialog.cpp
        appwindow.cpp
        chatwidget.cpp
        chatlogdialog.cpp
        settingsdialog.cpp
        usermodel.cpp
)

if (CMAKE_COMPILER_IS_GNUCXX)
    # Creates error in qendian.h, as it uses glibc's byteswap.
    REMOVE_DEFINITIONS( -Wold-style-cast )
endif(CMAKE_COMPILER_IS_GNUCXX)

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
