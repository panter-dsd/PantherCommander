TEMPLATE = app

CONFIG += qt \
	debug-and-release \
	warn_on
QT += gui \
	core

DESTDIR += bin
OBJECTS_DIR += build
MOC_DIR += build
UI_DIR += build

include( $$PWD/src/src.pri )
