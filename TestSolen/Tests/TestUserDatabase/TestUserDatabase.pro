QT += core gui sql testlib
CONFIG += c++17 testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../../Model

win32:CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../../Model/debug -lModel
} else {
    LIBS += -L$$OUT_PWD/../../Model/release -lModel
}

SOURCES += tst_userdatabase.cpp
