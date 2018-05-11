SET(TARGET PantherViewer)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)


SET(SOURCE
    AbstractView.cpp
    PantherViewer.cpp
    PlainView.cpp
    )

SET(HEADERS
    AbstractView.h
    PantherViewer.h
    PlainView.h
    )

ADD_LIBRARY(${TARGET} ${SOURCE} ${HEADERS})

TARGET_LINK_LIBRARIES(${TARGET} Qt5::Core Qt5::Gui Qt5::Widgets)

