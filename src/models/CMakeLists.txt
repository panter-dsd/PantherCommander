SET(TARGET models)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(SOURCE
    QFileListModel.cpp
    )

SET(HEADERS
    QFileListModel_p.h
    QFileListModel.h
    )

ADD_LIBRARY(${TARGET} ${SOURCE} ${HEADERS})

TARGET_LINK_LIBRARIES(${TARGET} Qt5::Core Qt5::Gui Qt5::Widgets)

