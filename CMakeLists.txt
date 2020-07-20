cmake_minimum_required (VERSION 3.10)

project (AmiKo-wx
	VERSION "0.1.79")

SET(CMAKE_CXX_STANDARD 17)

#=============================================================================

if (UNIX)
if (NOT APPLE)
# Linux
find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)

MESSAGE(STATUS "WEBKIT_Found: ${WEBKIT_FOUND}")
MESSAGE(STATUS "WEBKIT_VERSION: ${WEBKIT_VERSION}")
MESSAGE(STATUS "WEBKIT_LIBRARIES: ${WEBKIT_LIBRARIES}")
endif (NOT APPLE)
endif (UNIX)

#=============================================================================
message(STATUS "JSON_DIR: ${JSON_DIR}")
set(CMAKE_PREFIX_PATH ${JSON_DIR} ${CMAKE_PREFIX_PATH})
find_package(nlohmann_json 3.2.0 REQUIRED)

#=============================================================================

set(wxWidgets_ROOT_DIR ${WX_ROOT})

SET(wxWidgets_CONFIG_EXECUTABLE "${WX_ROOT}/bin/wx-config")
SET(wxWidgets_wxrc_EXECUTABLE "${WX_ROOT}/bin/wxrc")

if (APPLE)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

FIND_PACKAGE(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

if(wxWidgets_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
endif()
ADD_DEFINITIONS("-D__WXMAC__")
endif (APPLE)

if(UNIX)
if(NOT APPLE)
# Linux
FIND_PACKAGE(wxWidgets REQUIRED COMPONENTS core base net html xml webview)
ADD_DEFINITIONS("-D__WXGTK__")
endif(NOT APPLE)
endif(UNIX)

#MESSAGE(STATUS "wxWidgets_Found: ${wxWidgets_FOUND}")
#MESSAGE(STATUS "wxWidgets_ROOT_DIR: ${wxWidgets_ROOT_DIR}")
#MESSAGE(STATUS "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
#MESSAGE(STATUS "wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")

#=============================================================================

if (APPLE)
ADD_DEFINITIONS("-D_FILE_OFFSET_BITS=64")
ADD_DEFINITIONS("-D_LARGE_FILES")
ADD_DEFINITIONS("-DGL_SILENCE_DEPRECATION=1")
endif(APPLE)

#=============================================================================

set( SRCS_GUI
    gui/MainWindowBase.cpp gui/MainWindowBase.h
    gui/MainWindow.cpp gui/MainWindow.h
    gui/PatientSheet.cpp gui/PatientSheet.h
    gui/OperatorIDSheet.cpp gui/OperatorIDSheet.h )
    
set( SRCS_SQL
    DBAdapter.cpp DBAdapter.hpp
    SQLiteDatabase.cpp SQLiteDatabase.hpp
    FullTextDBAdapter.cpp FullTextDBAdapter.hpp )

set( SRCS_SQLITE
    sqlite/sqlite3.c sqlite/sqlite3.h sqlite/sqlite3ext.h )

set( SRCS_INTER
    InteractionsHtmlView.cpp InteractionsHtmlView.hpp
    InteractionsCart.cpp InteractionsCart.hpp
    InteractionsAdapter.cpp InteractionsAdapter.hpp
    )
    
set( SRCS_PRESCR
    PrescriptionsAdapter.cpp PrescriptionsAdapter.hpp
    PrescriptionItem.cpp PrescriptionItem.hpp
    PrescriptionsCart.cpp PrescriptionsCart.hpp
    ItemCellView.cpp ItemCellView.hpp
    )

source_group(GUI FILES ${SRCS_GUI})
source_group(SQL FILES ${SRCS_SQL})
source_group(sqlite FILES ${SRCS_SQLITE})
source_group(interactions FILES ${SRCS_INTER})
source_group(prescriptions FILES ${SRCS_PRESCR})

foreach(APP_NAME CoMed AmiKo)    
MESSAGE(STATUS "Configure target: ${APP_NAME}")

ADD_EXECUTABLE( ${APP_NAME} WIN32 MACOSX_BUNDLE
    main.cpp main.hpp
    Utilities.cpp Utilities.hpp
    Medication.cpp Medication.hpp
    Patient.cpp Patient.hpp
    Operator.cpp Operator.hpp
    TableViewDelegate.cpp TableViewDelegate.hpp
    FullTextSearch.cpp FullTextSearch.hpp
    FullTextEntry.cpp FullTextEntry.hpp
    DataStore.cpp DataStore.hpp
    ${SRCS_GUI} ${SRCS_SQL} ${SRCS_SQLITE} ${SRCS_INTER} ${SRCS_PRESCR}
    customURLConnection.cpp customURLConnection.hpp
    )

#ADD_DEFINITIONS("-DwxWidgets_USE_STATIC=OFF")
target_include_directories( ${APP_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/gui
    ${CMAKE_SOURCE_DIR}/sqlite
    ${wxWidgets_INCLUDE_DIRS} )
  
target_link_libraries( ${APP_NAME} ${wxWidgets_LIBRARIES} nlohmann_json::nlohmann_json)

if (UNIX)
if (APPLE)
  # macOS
  target_link_libraries( ${APP_NAME} ${wxWidgets_LIBRARY_DIRS}/libwx_osx_cocoau_webview-3.1.a)
else (APPLE)
  # Linux
  target_link_libraries( ${APP_NAME} ${WEBKIT_LIBRARIES})
endif (APPLE)
endif (UNIX)

target_compile_definitions(${APP_NAME} PUBLIC "APP_NAME=\"${APP_NAME}\"")

endforeach()

#=============================================================================

foreach(APP_NAME CoMed AmiKo) 
MESSAGE(STATUS "Install ${APP_NAME} to ${CMAKE_INSTALL_PREFIX}")
if (APPLE)
install(TARGETS ${APP_NAME} BUNDLE DESTINATION ${APP_NAME})
else (APPLE)
install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${APP_NAME})
endif (APPLE)

install(DIRECTORY lang
	DESTINATION ${APP_NAME}
	FILES_MATCHING PATTERN "*.mo")

install(FILES
    "${CMAKE_SOURCE_DIR}/res/amiko_stylesheet.css"
	"${CMAKE_SOURCE_DIR}/res/color-scheme-light.css"
    "${CMAKE_SOURCE_DIR}/res/color-scheme-dark.css"
    "${CMAKE_SOURCE_DIR}/res/fulltext_style.css"
    "${CMAKE_SOURCE_DIR}/res/interactions_callbacks.js"
    "${CMAKE_SOURCE_DIR}/res/interactions.css"
    "${CMAKE_SOURCE_DIR}/res/main_callbacks.js"
	DESTINATION ${APP_NAME})
endforeach()


