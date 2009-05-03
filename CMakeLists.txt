#Это необязательный, но желательный вызов, указывающий с помощью какой версии
#написан этот файл проекта
cmake_minimum_required( VERSION 2.6 )

# Подключение CMake модуля, отвечающего за библиотеку Qt4.
# Флаг REQUIRED означает, что если библиотека не была обнаружена -
#дальнейшее выполнение сборки невозможно.
# Подключение производится в два вызова find_package()/include() потому что
#между ними возможна настройка параметров библиотеки.
# Возможные параметры настройки Qt будут рассмотрены ниже, сейчас примите это как есть.

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
set( HEADERS src/mainwindowimpl.h src/qfilelistmodel.h src/qfilelistsortfilterproxymodel.h src/qfilelistview.h src/qfilepanel.h src/PantherViewer/pantherviewer.h src/PantherViewer/plainview.h src/PantherViewer/abstractview.h src/qfileoperationsdialog.h src/qfileoperationsthread.h src/qfullview.h src/qfileoperationsconfirmationdialog.h src/Preferences/qtoolbuttonpreference.h)
set( SOURCES src/mainwindowimpl.cpp src/main.cpp src/qfilelistmodel.cpp src/qfilelistsortfilterproxymodel.cpp src/qfilelistview.cpp src/qfilepanel.cpp src/PantherViewer/pantherviewer.cpp src/PantherViewer/plainview.cpp src/PantherViewer/abstractview.cpp src/qfileoperationsdialog.cpp src/qfileoperationsthread.cpp src/qfullview.cpp src/qfileoperationsconfirmationdialog.cpp src/Preferences/qtoolbuttonpreference.cpp)

#Убирание окна консоли
set(LINK_FLAGS "${LINK_FLAGS} -mwindows -mconsole")

# Выше вызов find_package(Qt4) обьявил набор вспомогательных функций для Qt4.
# Названия их начинаются с qt4_. Подробный список смотрите в документации по CMake.
# Одна из самых популярных функций - qt4_wrap_cpp() - генерирует MOC-файлы из
#заголовочников

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

# Создаём цель (target) myapp, которая в итоге представляет собой исполняемый файл,
#собранный из указанного списка исходников.

add_executable( PantherCommander ${SOURCES} ${MOC_SOURCES} )

# Линкуем наше приложение с библиотеками Qt.

target_link_libraries( PantherCommander ${QT_LIBRARIES} )

