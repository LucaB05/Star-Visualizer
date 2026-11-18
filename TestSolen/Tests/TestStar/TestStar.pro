QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    tst_teststar.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Model/release/ -lModel
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Model/debug/ -lModel
else:unix: LIBS += -L$$OUT_PWD/../../Model/ -lModel

INCLUDEPATH += $$PWD/../../Model
DEPENDPATH += $$PWD/../../Model

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Model/release/libModel.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Model/debug/libModel.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Model/release/Model.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Model/debug/Model.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Model/libModel.a
