INCLUDEPATH	+= $$PWD
DEPENDPATH	+= $$PWD

HEADERS	+= $$PWD/volumeinfoprovider.h \
	$$PWD/volumewatcher_p.h

SOURCES	+= $$PWD/volumeinfoprovider.cpp \
	$$PWD/volumewatcher.cpp

win32 {
	HEADERS	+= $$PWD/volumewatcher_win_p.h

	SOURCES	+= $$PWD/volumewatcher_win.cpp
} else {
	HEADERS	+= $$PWD/volumewatcher_unix_p.h

	SOURCES	+= $$PWD/volumewatcher_unix.cpp
}
