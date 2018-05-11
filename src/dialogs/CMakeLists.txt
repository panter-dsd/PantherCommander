SET(TARGET dialogs)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(dialogs_SOURCE
    FindFilesDialog.cpp
    PCCommandEditDialog.cpp
    PCCopyMoveDialog.cpp
    )

SET(dialogs_HEADERS
    FindFilesDialog.h
    PCCommandEditDialog.h
    PCCopyMoveDialog.h
    )

ADD_LIBRARY(${TARGET}
    ${dialogs_SOURCE}
    ${dialogs_HEADERS}
    )

TARGET_LINK_LIBRARIES(${TARGET} Qt5::Core Qt5::Gui Qt5::Widgets)

