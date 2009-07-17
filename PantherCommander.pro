TEMPLATE = app

include( config.pri )

include( $$PWD/src/src.pri )

win32 {
	INCLUDEPATH	+= $$PWD/exp
	DEPENDPATH	+= $$PWD/exp

	HEADERS	+= $$PWD/exp/filecontextmenu.h

	SOURCES	+= $$PWD/exp/filecontextmenu.cpp
}
