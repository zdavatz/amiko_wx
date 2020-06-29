cmake_minimum_required (VERSION 3.10)

project (AmiKo-wx
	VERSION "0.1.72")

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
if (UNIX)
if (NOT APPLE)
find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBKIT REQUIRED webkitgtk-3.0)
#pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)
endif (NOT APPLE)
endif (UNIX)

#MESSAGE(STATUS "WEBKIT_Found: ${WEBKIT_FOUND}")
#MESSAGE(STATUS "WEBKIT_VERSION: ${WEBKIT_VERSION}")

#=============================================================================

set( SRCS_GUI
    gui/MainWindowBase.cpp gui/MainWindowBase.h
    gui/MainWindow.cpp gui/MainWindow.h
    gui/PatientSheet.cpp gui/PatientSheet.h
    gui/OperatorIDSheet.cpp gui/OperatorIDSheet.h )
    
set( SRCS_SQL
    DBAdapter.cpp DBAdapter.hpp
    SQLiteDatabase.cpp SQLiteDatabase.hpp
    FullTextDBAdapter.cpp FullTextDBAdapter.hpp
    InteractionsAdapter.cpp InteractionsAdapter.hpp )

set( SRCS_SQLITE
    sqlite/sqlite3.c sqlite/sqlite3.h sqlite/sqlite3ext.h )

source_group(GUI FILES ${SRCS_GUI})
source_group(SQL FILES ${SRCS_SQL})
source_group(sqlite FILES ${SRCS_SQLITE})

foreach(APP_NAME CoMed AmiKo)    
MESSAGE(STATUS "Configure target: ${APP_NAME}")

ADD_EXECUTABLE( ${APP_NAME} WIN32 MACOSX_BUNDLE
    main.cpp main.hpp
    Utilities.cpp Utilities.hpp
    Medication.cpp Medication.hpp
    Patient.cpp Patient.hpp
    TableViewDelegate.cpp TableViewDelegate.hpp
    PrescriptionsAdapter.cpp PrescriptionsAdapter.hpp
    FullTextSearch.cpp FullTextSearch.hpp
    ${SRCS_GUI} ${SRCS_SQL} ${SRCS_SQLITE}
    customURLConnection.cpp customURLConnection.hpp
    )

#ADD_DEFINITIONS("-DwxWidgets_USE_STATIC=OFF")
target_include_directories( ${APP_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/gui
    ${CMAKE_SOURCE_DIR}/sqlite
    ${wxWidgets_INCLUDE_DIRS} )
  
target_link_libraries( ${APP_NAME}
  ${wxWidgets_LIBRARIES})

if (UNIX)
if (APPLE)
  # macOS
  target_link_libraries( ${APP_NAME} ${wxWidgets_LIBRARY_DIRS}/libwx_osx_cocoau_webview-3.1.a)
else (APPLE)
  # Linux
  target_link_libraries( ${APP_NAME} 
	${wxWidgets_LIBRARY_DIRS}/libwx_gtk3u_webview-3.1.a
	${WEBKIT_LIBRARIES}
	)
endif (APPLE)
endif (UNIX)

target_compile_definitions(${APP_NAME} PUBLIC "APP_NAME=\"${APP_NAME}\"")

endforeach()

#=============================================================================
foreach(APP_NAME CoMed AmiKo) 
MESSAGE(STATUS "Install ${APP_NAME} to ${CMAKE_INSTALL_PREFIX}")
if (APPLE)
install(TARGETS ${APP_NAME}	BUNDLE DESTINATION ${APP_NAME})
else (APPLE)
install(TARGETS ${APP_NAME}	RUNTIME DESTINATION ${APP_NAME})
endif (APPLE)

install(DIRECTORY lang
	DESTINATION ${APP_NAME}
	FILES_MATCHING PATTERN "*.mo")
endforeach()


