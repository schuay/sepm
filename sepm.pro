#-------------------------------------------------
#
# Project created by QtCreator 2012-05-01T12:45:09
#
#-------------------------------------------------

QT       += core gui

INCLUDEPATH += src/shared/ \
               src/client/backend

LIBS += -lIce -lIceUtil -lqca

TARGET = sepm
TEMPLATE = app


SOURCES +=\
    src/client/backend/chat.cpp \
    src/client/backend/user.cpp \
    src/client/backend/sessionmanager.cpp \
    src/client/backend/session.cpp \
    src/shared/Security.cpp \
    src/shared/sdcHelper.cpp \
    src/shared/QsLogDest.cpp \
    src/shared/QsLog.cpp \
    src/shared/QsDebugOutput.cpp \
    src/client/gui/logindialog.cpp \
    src/shared/SecureDistributedChat.cpp \
    src/client/main.cpp

HEADERS  += \
    src/client/backend/chat.h \
    src/client/backend/sessionmanager.h \
    src/client/backend/session.h \
    src/client/backend/user.h \
    src/shared/Security.h \
    src/shared/QsLogLevel.h \
    src/shared/QsLogDest.h \
    src/shared/QsLog.h \
    src/shared/QsDebugOutput.h \
    src/shared/sdcHelper.h \
    src/client/gui/logindialog.h \
    src/shared/SecureDistributedChat.h

FORMS    += \
    src/client/gui/logindialog.ui
