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
    src/AppSettings.cpp
    src/FileOperationThread.cpp
src/main.cpp
    src/MainWindowImpl.cpp
    src/PCCommands.cpp
    src/PCToolBar.cpp
    src/QFileOperationsConfirmationDialog.cpp
    src/QFileOperationsDialog.cpp
    src/QFileOperationsThread.cpp
    src/QFilePanel.cpp
	src/QFullView.cpp
	src/QSelectDiscDialog.cpp
)

set( src_HEADERS
    src/AppSettings.h
    src/FileOperationThread.h
    src/FileOperationThread_p.h
    src/MainWindowImpl.h
    src/PCCommands.h
    src/PCToolBar.h
    src/QFileOperationsConfirmationDialog.h
    src/QFileOperationsDialog.h
    src/QFileOperationsThread.h
    src/QFilePanel.h
	src/QFullView.h
	src/QSelectDiscDialog.h
)

################################################################
# dialogs
################################################################

set( dialogs_SOURCE
	src/dialogs/FindFilesDialog.cpp
    src/dialogs/PCCommandEditDialog.cpp
	src/dialogs/PCCopyMoveDialog.cpp
)

set( dialogs_HEADERS
	src/dialogs/FindFilesDialog.h
    src/dialogs/PCCommandEditDialog.h
	src/dialogs/PCCopyMoveDialog.h
)


################################################################
# io
################################################################

set( io_SOURCE
    src/io/VolumeInfoProvider.cpp
    src/io/VolumeWatcher.cpp
	src/io/VolumeWatcher_unix.cpp
#src/io/volumewatcher_win.cpp
)

set( io_HEADERS
    src/io/VolumeInfoProvider.h
    src/io/VolumeWatcher_p.h
    src/io/VolumeWatcher_unix_p.h
#src/io/volumewatcher_win_p.h
)


################################################################
# models
################################################################

set( models_SOURCE
	src/models/QFileListModel.cpp
)

set( models_HEADERS
    src/models/QFileListModel_p.h
	src/models/QFileListModel.h
)

################################################################
# PantherViewer
################################################################

set( PantherViewer_SOURCE
    src/PantherViewer/AbstractView.cpp
    src/PantherViewer/PantherViewer.cpp
	src/PantherViewer/PlainView.cpp
)

set( PantherViewer_HEADERS
    src/PantherViewer/AbstractView.h
    src/PantherViewer/PantherViewer.h
	src/PantherViewer/PlainView.h
)


################################################################
# Preferences
################################################################

set( Preferences_SOURCE
    src/Preferences/PCCommandsDialog.cpp
    src/Preferences/PCCommandsPreference.cpp
    src/Preferences/QAbstractPreferencesPage.cpp
    src/Preferences/QInterfacePreference.cpp
	src/Preferences/QPreferenceGlobal.cpp
    src/Preferences/QPreferencesDialog.cpp
    src/Preferences/QToolButtonPreference.cpp
)

set( Preferences_HEADERS
    src/Preferences/PCCommandsDialog.h
    src/Preferences/PCCommandsPreference.h
    src/Preferences/QAbstractPreferencesPage.h
    src/Preferences/QInterfacePreference.h
	src/Preferences/QPreferenceGlobal.h
    src/Preferences/QPreferencesDialog.h
    src/Preferences/QToolButtonPreference.h
)


################################################################
# tools
################################################################

set( tools_SOURCE
    src/tools/DirSorter.cpp
)

set( tools_HEADERS
    src/tools/DirSorter.h
)


################################################################
# widgets
################################################################

set( widgets_SOURCE
    src/widgets/DriveBar.cpp
    src/widgets/FileWidget.cpp
	src/widgets/FlowLayout.cpp
    src/widgets/PCShortcutEdit.cpp
    src/widgets/TabBar.cpp
)

set( widgets_HEADERS
    src/widgets/DriveBar.h
    src/widgets/FileWidget.h
    src/widgets/FileWidget_p.h
	src/widgets/FlowLayout.h
    src/widgets/PCShortcutEdit.h
    src/widgets/TabBar.h
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

