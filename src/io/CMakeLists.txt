SET(TARGET io)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCE
    VolumeInfoProvider.cpp
    VolumeWatcher.cpp
    VolumeWatcher_unix.cpp
    )

SET(HEADERS
    VolumeInfoProvider.h
    VolumeWatcher_p.h
    VolumeWatcher_unix_p.h
    )

ADD_LIBRARY(${TARGET} ${SOURCE} ${HEADERS})

TARGET_LINK_LIBRARIES(${TARGET} Qt5::Core Qt5::Gui Qt5::Widgets)

