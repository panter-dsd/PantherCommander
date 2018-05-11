SET(TARGET Preferences)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Widgets)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

################################################################
# Preferences
################################################################

SET(SOURCE
    CommandsDialog.cpp
    CommandsPreference.cpp
    AbstractPreferencesPage.cpp
    InterfacePreference.cpp
    QPreferenceGlobal.cpp
    QPreferencesDialog.cpp
    QToolButtonPreference.cpp
    )

SET(HEADERS
    CommandsDialog.h
    CommandsPreference.h
    AbstractPreferencesPage.h
    InterfacePreference.h
    QPreferenceGlobal.h
    QPreferencesDialog.h
    QToolButtonPreference.h
    )


ADD_LIBRARY(${TARGET} ${SOURCE} ${HEADERS})

TARGET_LINK_LIBRARIES(${TARGET} Qt5::Core Qt5::Gui Qt5::Widgets)

