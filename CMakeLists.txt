project(PantherCommander)
cmake_minimum_required( VERSION 2.6 )

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
exp
src
src/PantherViewer
src/Preferences
src/dialogs
src/io
src/models
src/tools
src/widgets
${QT_MKSPECS_DIR}/default
${CMAKE_CURRENT_SOURCE_DIR}
)

################################################################
# exp
################################################################

set( exp_SOURCE
#exp/filecontextmenu.cpp
)

set( exp_HEADERS
#exp/filecontextmenu.h
)



################################################################
# src
################################################################

set( src_SOURCE
src/appsettings.cpp
src/fileoperationthread.cpp
src/main.cpp
src/mainwindowimpl.cpp
src/pccommands.cpp
src/pctoolbar.cpp
src/qfileoperationsconfirmationdialog.cpp
src/qfileoperationsdialog.cpp
src/qfileoperationsthread.cpp
src/qfilepanel.cpp
src/qfullview.cpp
src/qselectdiscdialog.cpp
)

set( src_HEADERS
src/appsettings.h
src/fileoperationthread.h
src/fileoperationthread_p.h
src/mainwindowimpl.h
src/pccommands.h
src/pctoolbar.h
src/qfileoperationsconfirmationdialog.h
src/qfileoperationsdialog.h
src/qfileoperationsthread.h
src/qfilepanel.h
src/qfullview.h
src/qselectdiscdialog.h
)

################################################################
# dialogs
################################################################

set( dialogs_SOURCE
src/dialogs/findfilesdialog.cpp
src/dialogs/pccommandeditdialog.cpp
src/dialogs/pccopymovedialog.cpp
)

set( dialogs_HEADERS
src/dialogs/findfilesdialog.h
src/dialogs/pccommandeditdialog.h
src/dialogs/pccopymovedialog.h
)


################################################################
# io
################################################################

set( io_SOURCE
src/io/volumeinfoprovider.cpp
src/io/volumewatcher.cpp
src/io/volumewatcher_unix.cpp
#src/io/volumewatcher_win.cpp
)

set( io_HEADERS
src/io/volumeinfoprovider.h
src/io/volumewatcher_p.h
src/io/volumewatcher_unix_p.h
#src/io/volumewatcher_win_p.h
)


################################################################
# models
################################################################

set( models_SOURCE
src/models/qfilelistmodel.cpp
)

set( models_HEADERS
src/models/qfilelistmodel_p.h
src/models/qfilelistmodel.h
)

################################################################
# PantherViewer
################################################################

set( PantherViewer_SOURCE
src/PantherViewer/abstractview.cpp
src/PantherViewer/pantherviewer.cpp
src/PantherViewer/plainview.cpp
)

set( PantherViewer_HEADERS
src/PantherViewer/abstractview.h
src/PantherViewer/pantherviewer.h
src/PantherViewer/plainview.h
)


################################################################
# Preferences
################################################################

set( Preferences_SOURCE
src/Preferences/pccommandsdialog.cpp
src/Preferences/pccommandspreference.cpp
src/Preferences/qabstractpreferencespage.cpp
src/Preferences/qinterfacepreference.cpp
src/Preferences/qpreferenceglobal.cpp
src/Preferences/qpreferencesdialog.cpp
src/Preferences/qtoolbuttonpreference.cpp
)

set( Preferences_HEADERS
src/Preferences/pccommandsdialog.h
src/Preferences/pccommandspreference.h
src/Preferences/qabstractpreferencespage.h
src/Preferences/qinterfacepreference.h
src/Preferences/qpreferenceglobal.h
src/Preferences/qpreferencesdialog.h
src/Preferences/qtoolbuttonpreference.h
)


################################################################
# tools
################################################################

set( tools_SOURCE
src/tools/dirsorter.cpp
)

set( tools_HEADERS
src/tools/dirsorter.h
)


################################################################
# widgets
################################################################

set( widgets_SOURCE
src/widgets/drivebar.cpp
src/widgets/filewidget.cpp
src/widgets/flowlayout.cpp
src/widgets/pcshortcutedit.cpp
src/widgets/tabbar.cpp
)

set( widgets_HEADERS
src/widgets/drivebar.h
src/widgets/filewidget.h
src/widgets/filewidget_p.h
src/widgets/flowlayout.h
src/widgets/pcshortcutedit.h
src/widgets/tabbar.h
)

add_executable( PantherCommander
	${exp_SOURCE}
        ${exp_HEADERS}
	${src_SOURCE}
        ${src_HEADERS}
	${models_SOURCE}
        ${models_HEADERS}
	${dialogs_SOURCE}
        ${dialogs_HEADERS}
	${io_SOURCE}
        ${io_HEADERS}
	${PantherViewer_SOURCE}
        ${PantherViewer_HEADERS}
	${Preferences_SOURCE}
        ${Preferences_HEADERS}
	${tools_SOURCE}
        ${tools_HEADERS}
	${widgets_SOURCE}
        ${widgets_HEADERS}
 )


target_link_libraries( PantherCommander Qt5::Core Qt5::Gui Qt5::Widgets)

