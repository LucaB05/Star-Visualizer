QT += core gui widgets testlib sql 3dcore 3drender 3dinput 3dextras
CONFIG += c++17 testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../../Controller
INCLUDEPATH += $$PWD/../../Model
INCLUDEPATH += $$PWD/../../View


win32:CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../../Controller/debug -lController
    LIBS += -L$$OUT_PWD/../../Model/debug -lModel
    LIBS += -L$$OUT_PWD/../../View/debug -lView
} else {
    LIBS += -L$$OUT_PWD/../../Controller/release -lController
    LIBS += -L$$OUT_PWD/../../Model/release -lModel
    LIBS += -L$$OUT_PWD/../../View/release -lView
}

SOURCES +=  tst_test_star_search.cpp
