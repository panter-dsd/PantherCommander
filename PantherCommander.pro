TEMPLATE = app

CONFIG += qt \
	debug-and-release \
	warn_on
QT += gui \
	core

DESTDIR += bin

win32 {
	OBJECTS_DIR += build_win
	MOC_DIR += build_win
	UI_DIR += build_win
}

unix {
	OBJECTS_DIR += build_unix
	MOC_DIR += build_unix
	UI_DIR += build_unix
}

macx {
	OBJECTS_DIR += build_macx
	MOC_DIR += build_macx
	UI_DIR += build_macx
}
include( $$PWD/src/src.pri )
