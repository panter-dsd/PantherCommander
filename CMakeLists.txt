PROJECT(PantherCommander)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra")

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_SUBDIRECTORY(src/dialogs)
ADD_SUBDIRECTORY(src/io)
ADD_SUBDIRECTORY(src/models)
ADD_SUBDIRECTORY(src/PantherViewer)
ADD_SUBDIRECTORY(src/Preferences)
ADD_SUBDIRECTORY(src/tools)
ADD_SUBDIRECTORY(src/widgets)

SET(src_SOURCE
    src/AppSettings.cpp
    src/FileOperationThread.cpp
    src/main.cpp
    src/MainWindow.cpp
    src/Commands.cpp
    src/ToolBar.cpp
    src/FileOperationsConfirmationDialog.cpp
    src/QFileOperationsDialog.cpp
    src/QFileOperationsThread.cpp
    src/QFilePanel.cpp
    src/QFullView.cpp
    src/QSelectDiscDialog.cpp
    )

SET(src_HEADERS
    src/AppSettings.h
    src/FileOperations.h
    src/FileOperationThread.h
    src/FileOperationThread_p.h
    src/MainWindow.h
    src/Commands.h
    src/ToolBar.h
    src/FileOperationsConfirmationDialog.h
    src/QFileOperationsDialog.h
    src/QFileOperationsThread.h
    src/QFilePanel.h
    src/QFullView.h
    src/QSelectDiscDialog.h
    )


ADD_EXECUTABLE(PantherCommander
    ${exp_SOURCE}
    ${exp_HEADERS}
    ${src_SOURCE}
    ${src_HEADERS}
    )


TARGET_LINK_LIBRARIES(PantherCommander
    dialogs
    io
    models
    PantherViewer
    Preferences
    tools
    widgets
    Qt5::Core Qt5::Gui Qt5::Widgets)

