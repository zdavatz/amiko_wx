cmake_minimum_required (VERSION 3.10)

project (AmiKo-wx)

SET(CMAKE_CXX_STANDARD 17)

#-----------------------------------------------------------------------------
set(wxWidgets_ROOT_DIR ${WX_ROOT})

if (APPLE)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(wxWidgets_CONFIG_EXECUTABLE "${WX_ROOT}/bin/wx-config")
SET(wxWidgets_wxrx_EXECUTABLE "${WX_ROOT}/bin/wxrc")
FIND_PACKAGE(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

if(wxWidgets_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
endif()
endif (APPLE)

if(NOT APPLE)
if(UNIX)
SET(wxWidgets_CONFIG_EXECUTABLE "${WX_ROOT}/bin/wx-config")
SET(wxWidgets_wxrc_EXECUTABLE "${WX_ROOT}/bin/wxrc")
FIND_PACKAGE(wxWidgets REQUIRED )
endif(UNIX)
endif(NOT APPLE)

if (APPLE)
# From wx-config
ADD_DEFINITIONS("-D_FILE_OFFSET_BITS=64")
ADD_DEFINITIONS("-D_LARGE_FILES")
ADD_DEFINITIONS("-D__WXMAC__")
ADD_DEFINITIONS("-DGL_SILENCE_DEPRECATION=1")
endif(APPLE)

if(NOT APPLE)
if(UNIX)
ADD_DEFINITIONS("-D__WXGTK__")
endif(UNIX)
endif(NOT APPLE)

#=============================================================================
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
message(STATUS "SQLITE3_INCLUDEDIR: ${SQLITE3_INCLUDEDIR}")
message(STATUS "SQLITE3_LIBRARIES: ${SQLITE3_LIBRARIES}")
message(STATUS "SQLite3_FOUND: ${SQLite3_FOUND}")

#=============================================================================

foreach(APP_NAME CoMed AmiKo)

set( SRCS_GUI
    gui/MainWindowBase.cpp gui/MainWindowBase.h
    gui/MainWindow.cpp gui/MainWindow.h
    gui/PatientSheet.cpp gui/PatientSheet.h)
    
set( SRCS_SQL
    DBAdapter.cpp DBAdapter.hpp
    SQLiteDatabase.cpp SQLiteDatabase.hpp
    FullTextDBAdapter.cpp FullTextDBAdapter.hpp
    InteractionsAdapter.cpp InteractionsAdapter.hpp )
    
source_group(GUI FILES ${SRCS_GUI})
source_group(SQL FILES ${SRCS_SQL})

MESSAGE("=== TARGET: ${APP_NAME}")

ADD_EXECUTABLE( ${APP_NAME} WIN32 MACOSX_BUNDLE
    main.cpp main.hpp
    Utilities.cpp Utilities.hpp
    Medication.cpp Medication.hpp
    TableViewDelegate.cpp TableViewDelegate.hpp
    PrescriptionsAdapter.cpp PrescriptionsAdapter.hpp
    OperatorIDSheetController.cpp OperatorIDSheetController.hpp
    ${SRCS_GUI} ${SRCS_SQL}
    customURLConnection.cpp customURLConnection.hpp
    )

#ADD_DEFINITIONS("-DwxWidgets_USE_STATIC=OFF")
target_include_directories( ${APP_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/gui
    ${wxWidgets_INCLUDE_DIRS}
    ${SQLITE3_INCLUDEDIR} )

target_link_libraries( ${APP_NAME}
  ${wxWidgets_LIBRARIES}
  ${SQLITE3_LIBRARIES})

target_compile_definitions(${APP_NAME} PUBLIC "APP_NAME=\"${APP_NAME}\"")

endforeach(loop_var)

