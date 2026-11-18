
QT += core gui widgets 3dcore 3drender 3dinput 3dextras
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AppNavigator.cpp \
    StarView.cpp \
    adminview.cpp \
    createaccountview.cpp \
    loginview.cpp

HEADERS += \
    AppNavigator.h \
    StarView.h \
    adminview.h \
    createaccountview.h \
    loginview.h

INCLUDEPATH += $$PWD/../Model \

DEPENDPATH += $$PWD/../Model \

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    StarView.ui \
    adminview.ui \
    createAccount.ui \
    logIn.ui
