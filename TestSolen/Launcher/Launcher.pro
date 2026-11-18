
QT += core gui widgets 3dcore 3drender 3dinput 3dextras sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp

HEADERS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Controller/release/ -lController
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Controller/debug/ -lController
else:unix: LIBS += -L$$OUT_PWD/../Controller/ -lController

INCLUDEPATH += $$PWD/../Controller
DEPENDPATH += $$PWD/../Controller

INCLUDEPATH += $$PWD/../Model
DEPENDPATH += $$PWD/../Model

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Controller/release/libController.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Controller/debug/libController.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Controller/release/Controller.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Controller/debug/Controller.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Controller/libController.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../View/release/ -lView
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../View/debug/ -lView
else:unix: LIBS += -L$$OUT_PWD/../View/ -lView

INCLUDEPATH += $$PWD/../View
DEPENDPATH += $$PWD/../View

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../View/release/libView.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../View/debug/libView.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../View/release/View.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../View/debug/View.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../View/libView.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Model/release/ -lModel
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Model/debug/ -lModel
else:unix: LIBS += -L$$OUT_PWD/../Model/ -lModel

INCLUDEPATH += $$PWD/../Model
DEPENDPATH += $$PWD/../Model


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Model/release/libModel.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Model/debug/libModel.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Model/release/Model.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Model/debug/Model.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Model/libModel.a
