load(qttest_p4)
SOURCES += tst_zip.cpp

include( $$PWD/../../src/3dparty/qzip/qzip.pri )

wince*: {
   addFiles.sources = testdata
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}
