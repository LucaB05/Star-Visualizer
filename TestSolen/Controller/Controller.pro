
QT += core gui widgets sql
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AdminController.cpp \
    AuthController.cpp \
    StarController.cpp

HEADERS += \
    AdminController.h \
    AuthController.h \
    StarController.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../View/release/ -lView
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../View/debug/ -lView
else:unix: LIBS += -L$$OUT_PWD/../View/ -lView

INCLUDEPATH += $$PWD/../Model \
               $$PWD/../View

DEPENDPATH += $$PWD/../Model \
              $$PWD/..View


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
