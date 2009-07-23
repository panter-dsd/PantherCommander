INCLUDEPATH	+= $$PWD
DEPENDPATH	+= $$PWD

HEADERS	+= $$PWD/appsettings.h \
		$$PWD/mainwindowimpl.h \
		$$PWD/qfilepanel.h \
		$$PWD/qfileoperationsdialog.h \
		$$PWD/qfileoperationsthread.h \
		$$PWD/qfullview.h \
		$$PWD/qfileoperationsconfirmationdialog.h \
		$$PWD/qselectdiscdialog.h \
		$$PWD/pctoolbar.h \
		$$PWD/pccommands.h

SOURCES	+= $$PWD/main.cpp \
		$$PWD/appsettings.cpp \
		$$PWD/mainwindowimpl.cpp \
		$$PWD/qfilepanel.cpp \
		$$PWD/qfileoperationsdialog.cpp \
		$$PWD/qfileoperationsthread.cpp \
		$$PWD/qfullview.cpp \
		$$PWD/qfileoperationsconfirmationdialog.cpp \
		$$PWD/qselectdiscdialog.cpp \
		$$PWD/pctoolbar.cpp \
		$$PWD/pccommands.cpp

include( $$PWD/dialogs/dialogs.pri )
include( $$PWD/io/io.pri )
include( $$PWD/models/models.pri )
include( $$PWD/PantherViewer/pantherviewer.pri )
include( $$PWD/Preferences/preferences.pri )
include( $$PWD/tools/tools.pri )
include( $$PWD/widgets/widgets.pri )

LIBS	*= $$QMAKE_LIBS_CORE
