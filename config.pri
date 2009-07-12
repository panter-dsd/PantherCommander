CONFIG += thread \
	debug-and-release \
	warn_on
CONFIG -= exceptions rtti
QT *= core gui

DESTDIR		= bin

BUILDDIR	= build/
win32:BUILDDIR	= $$join(BUILDDIR,,,win32)
unix:BUILDDIR	= $$join(BUILDDIR,,,unix)
macx:BUILDDIR	= $$join(BUILDDIR,,,macx)

UI_DIR		= $${BUILDDIR}/ui
UIC_DIR		= $${BUILDDIR}/uic
MOC_DIR		= $${BUILDDIR}/moc
RCC_DIR		= $${BUILDDIR}/rcc
OBJECTS_DIR	= $${BUILDDIR}/obj

CONFIG(release, debug|release) {
	OBJECTS_DIR = $$join(OBJECTS_DIR,,,/release)
}
else {
	DEFINES += DEBUG
	OBJECTS_DIR = $$join(OBJECTS_DIR,,,/debug)
}

# precompiled headers breaks icecream builds for some reason :(
unix:system(test `ps aux | grep iceccd | wc -l` -gt 1) {
	CONFIG -= precompile_header
}

#universal binaries cannot be built from precompiled headers
mac*:CONFIG(release, debug|release) {
	macx-g++:CONFIG += x86 ppc
	CONFIG -= precompile_header
}

win32-g++ {
	QMAKE_CFLAGS_DEBUG		= -O0 -g
	QMAKE_CXXFLAGS_DEBUG	= -O0 -g
	QMAKE_CFLAGS_RELEASE	= -O3 -fomit-frame-pointer
	QMAKE_CXXFLAGS_RELEASE	= -O3 -fomit-frame-pointer
}
