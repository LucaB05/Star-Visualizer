TEMPLATE = subdirs

SUBDIRS += \
    Model \
    View \
    Controller \
    Tests \
    Launcher

Tests.depends = Model View Controller
Controller.depends = Model View
Launcher.depends = Model View Controller
