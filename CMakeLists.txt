cmake_minimum_required(VERSION 2.8)
project(disgrace)

set(DISGRACE_VERSION 0)

OPTION(G "Set to ON to compile with optimisations and debug symbols" OFF)

IF(G)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo)
	MESSAGE(STATUS "Compiling in Release+Debug symbols mode")
ENDIF(G)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DQT_MESSAGELOGCONTEXT -Wall -DDISGRACE_VERSION=${DISGRACE_VERSION}) 

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Boost 1.45.0 REQUIRED)

# The Qt5Widgets_INCLUDES also includes the include directories for dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# This allows ui_MainWindow.h to find qcustomplot.h
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/qcustomplot)

# Boost's headers
include_directories(${Boost_INCLUDE_DIRS})

set(QT5_LIBRARIES Qt5::Widgets Qt5::PrintSupport)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(SOURCES
	src/qcustomplot/qcustomplot.cpp
	src/main.cpp
	src/MainWindow.cpp
	src/Commands/Legend.cpp
	src/Commands/Axis.cpp
	src/Commands/Set.cpp
	src/Dialogs/ImportDataset.cpp
	src/Dialogs/SetAppearance.cpp
	src/AgrObjects/AgrFile.cpp
	src/AgrObjects/Dataset.cpp
	src/AgrObjects/AgrRegion.cpp
	src/AgrObjects/AgrDrawingObject.cpp
	src/AgrObjects/AgrGraph.cpp
	src/AgrObjects/AgrSettings.cpp
	src/AgrObjects/AgrDefaults.cpp
)

set(UI_SOURCES
	src/MainWindow.ui
	src/Dialogs/ImportDataset.ui
	src/Dialogs/SetAppearance.ui
)

qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES})

add_executable(disgrace ${SOURCES} ${UI_GENERATED_HEADERS})

target_link_libraries(disgrace ${QT5_LIBRARIES})
