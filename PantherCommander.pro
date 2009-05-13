TEMPLATE = app
QT += gui \
    core
CONFIG += qt \
    release \
    warn_on \
    exceptions \
    console
DESTDIR += bin
OBJECTS_DIR += build
MOC_DIR += build
UI_DIR += build
HEADERS += src/mainwindowimpl.h \
    src/qfilelistmodel.h \
    src/qfilelistsortfilterproxymodel.h \
    src/qfilelistview.h \
    src/qfilepanel.h \
    src/PantherViewer/pantherviewer.h \
    src/PantherViewer/plainview.h \
    src/PantherViewer/abstractview.h \
    src/qfileoperationsdialog.h \
    src/qfileoperationsthread.h \
    src/qfullview.h \
    src/qfileoperationsconfirmationdialog.h \
    src/Preferences/qtoolbuttonpreference.h
SOURCES += src/mainwindowimpl.cpp \
    src/main.cpp \
    src/qfilelistmodel.cpp \
    src/qfilelistsortfilterproxymodel.cpp \
    src/qfilelistview.cpp \
    src/qfilepanel.cpp \
    src/PantherViewer/pantherviewer.cpp \
    src/PantherViewer/plainview.cpp \
    src/PantherViewer/abstractview.cpp \
    src/qfileoperationsdialog.cpp \
    src/qfileoperationsthread.cpp \
    src/qfullview.cpp \
    src/qfileoperationsconfirmationdialog.cpp \
    src/Preferences/qtoolbuttonpreference.cpp
