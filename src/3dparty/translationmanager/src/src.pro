#!win32:VERSION	= 0.1

TEMPLATE	= lib
LANGUAGE	= C++

DEFINES		*= TRANSLATIONMANAGER_BUILD

include(../config.pri)
include(../translationmanager.pri)

TARGET		= $$qtLibraryTarget($$TRANSLATIONMANAGER_LIBNAME)

CONFIG		+= $$TRANSLATIONMANAGER_LIBCONFIG
QT		-= gui

include(src.pri)

win32:CLEAN_FILES += $(DLLDESTDIR)/$(QMAKE_TARGET).lib $(DLLDESTDIR)/$(QMAKE_TARGET).dll
else:CLEAN_FILES += $(DESTDIR)/lib$(QMAKE_TARGET)*
