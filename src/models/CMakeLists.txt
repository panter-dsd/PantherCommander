SET(TARGET models)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCE
    FileListModel.cpp
    )

SET(HEADERS
    FileListModel_p.h
    FileListModel.h
    )

ADD_LIBRARY(${TARGET} ${SOURCE} ${HEADERS})

TARGET_LINK_LIBRARIES(${TARGET} Qt5::Core Qt5::Gui Qt5::Widgets)

