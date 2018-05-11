SET(TARGET widgets)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCE
    DriveBar.cpp
    FileWidget.cpp
    FlowLayout.cpp
    ShortcutEdit.cpp
    TabBar.cpp
    )

SET(HEADERS
    DriveBar.h
    FileWidget.h
    FileWidget_p.h
    FlowLayout.h
    ShortcutEdit.h
    TabBar.h
    )


ADD_LIBRARY(${TARGET} ${SOURCE} ${HEADERS})

TARGET_LINK_LIBRARIES(${TARGET} models Qt5::Core Qt5::Gui Qt5::Widgets)

