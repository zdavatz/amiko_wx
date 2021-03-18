cmake_minimum_required (VERSION 3.10)

project (AmiKo-wx
	VERSION "0.1.277")

SET(CMAKE_CXX_STANDARD 17)

configure_file(
    "${CMAKE_SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h")
include_directories(${CMAKE_BINARY_DIR})

#===============================================================================

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

#===============================================================================
message(STATUS "JSON_DIR: ${JSON_DIR}")
set(CMAKE_PREFIX_PATH ${JSON_DIR} ${CMAKE_PREFIX_PATH})
find_package(nlohmann_json 3.2.0 REQUIRED)

#===============================================================================
message(STATUS "CURL_DIR: ${CURL_DIR}")
set(CMAKE_PREFIX_PATH ${CURL_DIR} ${CMAKE_PREFIX_PATH})
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
find_package(CURL 7.73.0 REQUIRED)

#===============================================================================
if (UNIX)
if (NOT APPLE)
# Linux
pkg_check_modules(OPENSSL REQUIRED openssl)
MESSAGE(STATUS "OPENSSL_Found: ${OPENSSL_FOUND}")
else (NOT APPLE)
set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
find_package(OpenSSL REQUIRED)
endif (NOT APPLE)
endif (UNIX)

#===============================================================================
if (UNIX)
if (NOT APPLE)
# Linux
PKG_CHECK_MODULES(PCSC libpcsclite REQUIRED)
MESSAGE(STATUS "PCSC_FOUND: ${PCSC_FOUND}")
MESSAGE(STATUS "PCSC_INCLUDE_DIRS: ${PCSC_INCLUDE_DIRS}")
MESSAGE(STATUS "PCSC_LIBRARIES: ${PCSC_LIBRARIES}")

FIND_PATH(PCSC_INCLUDE_DIR winscard.h
	HINTS
	/usr/include/PCSC
	PATH_SUFFIXES PCSC
	)

FIND_LIBRARY(PCSC_LIBRARY NAMES pcsclite libpcsclite PCSC
	HINTS
	${PC_PCSC_LIBDIR}
	${PC_PCSC_LIBRARY_DIRS}
	)

MESSAGE(STATUS "PCSC_INCLUDE_DIR: ${PCSC_INCLUDE_DIR}")
MESSAGE(STATUS "PCSC_LIBRARY: ${PCSC_LIBRARY}")
endif (NOT APPLE)
endif (UNIX)

#===============================================================================

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

#===============================================================================

set( SRCS_GUI
    gui/MainWindowBase.cpp gui/MainWindowBase.h
    gui/MainWindow.cpp gui/MainWindow.h
    gui/SplashWindow.cpp gui/SplashWindow.h
		)
    
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

set( SRCS_PATIENT
    gui/PatientSheet.cpp gui/PatientSheet.h
    Patient.cpp Patient.hpp
    Contacts.cpp Contacts.hpp
    PatientDBAdapter.cpp PatientDBAdapter.hpp
    )

set( SRCS_PREFERENCES
    gui/PreferencesWindow.cpp gui/PreferencesWindow.hpp
    gui/GoogleAuthSheet.cpp gui/GoogleAuthSheet.h
    sync/GoogleSyncManager.cpp sync/GoogleSyncManager.hpp
    )

set( SRCS_DOCTOR
    gui/OperatorIDSheet.cpp gui/OperatorIDSheet.h
    Operator.cpp Operator.hpp
		SignatureView.cpp SignatureView.hpp
		)

set( SRCS_CSS
    res/amiko_stylesheet.css
    res/color-scheme-light.css
    res/color-scheme-dark.css
    res/fulltext_style.css
    res/interactions.css
	)

set( SRCS_JS
    res/interactions_callbacks.js
    res/main_callbacks.js
	)

set( SRCS_LANG
    lang/de_CH/amiko.po
    lang/fr_CH/amiko.po
    )
    
set( SRCS_CARD
    card/HealthCard.cpp card/HealthCard.hpp
    card/SmartCard.cpp card/SmartCard.hpp
    )

set( SRCS_PRINT
    print/LabelPrintout.cpp print/LabelPrintout.hpp
    print/PrescriptionPrintout.cpp print/PrescriptionPrintout.hpp
    print/MainWindowPrint.cpp
    )
    
source_group(GUI FILES ${SRCS_GUI})
source_group(SQL FILES ${SRCS_SQL})
source_group(sqlite FILES ${SRCS_SQLITE})
source_group(interactions FILES ${SRCS_INTER})
source_group(prescriptions FILES ${SRCS_PRESCR})
source_group(patients FILES ${SRCS_PATIENT})
source_group(preferences FILES ${SRCS_PREFERENCES})
source_group(doctor FILES ${SRCS_DOCTOR})
source_group("Resources/css" FILES ${SRCS_CSS})
source_group("Resources/javascript" FILES ${SRCS_JS})
source_group(lang FILES ${SRCS_LANG})
source_group(card FILES ${SRCS_CARD})
source_group(print FILES ${SRCS_PRINT})

set(MY_TARGETS AmiKo CoMed)
set(MY_LANGUAGES de_CH fr_CH)

foreach(APP_NAME ${MY_TARGETS}) 
MESSAGE(STATUS "Configure target: ${APP_NAME}")

add_definitions(-DAMIKO_GOOGLE_CLIENT_ID="$ENV{AMIKO_GOOGLE_CLIENT_ID}")
add_definitions(-DAMIKO_GOOGLE_CLIENT_SECRET="$ENV{AMIKO_GOOGLE_CLIENT_SECRET}")
add_definitions(-DCOMED_GOOGLE_CLIENT_ID="$ENV{COMED_GOOGLE_CLIENT_ID}")
add_definitions(-DCOMED_GOOGLE_CLIENT_SECRET="$ENV{COMED_GOOGLE_CLIENT_SECRET}")

if (APPLE)
  set(MAC_APP_ICON res/mac/${APP_NAME}.icns)
  set(MAC_LANG
    lang/de_CH/LC_MESSAGES/amiko.mo
    lang/fr_CH/LC_MESSAGES/amiko.mo
    )

  set_source_files_properties(
    ${SRCS_CSS} ${SRCS_JS} ${MAC_APP_ICON}
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    
  foreach(LANG_NAME ${MY_LANGUAGES})
    set_source_files_properties(
        lang/${LANG_NAME}/LC_MESSAGES/amiko.mo
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources/${LANG_NAME}.lproj)
  endforeach()

  source_group(Resources FILES ${MAC_APP_ICON})
  #include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif (APPLE)

ADD_EXECUTABLE( ${APP_NAME} WIN32 MACOSX_BUNDLE
    main.cpp main.hpp
    Utilities.cpp Utilities.hpp
    DefaultsController.cpp DefaultsController.hpp
    Medication.cpp Medication.hpp
    TableViewDelegate.cpp TableViewDelegate.hpp
    FullTextSearch.cpp FullTextSearch.hpp
    FullTextEntry.cpp FullTextEntry.hpp
    DataStore.cpp DataStore.hpp
    ${SRCS_PRINT}
    ${SRCS_GUI} ${SRCS_SQL} ${SRCS_SQLITE}
    ${SRCS_INTER} ${SRCS_PRESCR} ${SRCS_PATIENT}
		${SRCS_DOCTOR} ${SRCS_CSS} ${SRCS_JS} ${SRCS_CARD}
    ${SRCS_PREFERENCES}
    customURLConnection.cpp customURLConnection.hpp
    ${SRCS_LANG} ${MAC_APP_ICON} ${MAC_LANG}
    )

#ADD_DEFINITIONS("-DwxWidgets_USE_STATIC=OFF")
target_include_directories( ${APP_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/card
    ${CMAKE_SOURCE_DIR}/gui
    ${CMAKE_SOURCE_DIR}/print
    ${CMAKE_SOURCE_DIR}/sqlite
    ${wxWidgets_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR} )
  
target_link_libraries( ${APP_NAME}
		${wxWidgets_LIBRARIES}
		${OPENSSL_LIBRARIES}
        ${CURL_LIBRARIES}
		nlohmann_json::nlohmann_json)

# macOS
if (APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
  target_link_libraries( ${APP_NAME}
    ${wxWidgets_LIBRARY_DIRS}/libwx_osx_cocoau_webview-3.1.a
    "-framework PCSC")

  set_target_properties(${APP_NAME} PROPERTIES
    #MACOSX_BUNDLE_ICON_FILE ${MAC_APP_ICON}
    MACOSX_BUNDLE_ICON_FILE ${APP_NAME}
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.ywesee.${APP_NAME}"
    MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
    )
  

endif (APPLE)

# Linux
if (NOT APPLE)
if (UNIX)
  target_include_directories( ${APP_NAME} PUBLIC ${PCSC_INCLUDE_DIR})
  target_link_libraries( ${APP_NAME} ${WEBKIT_LIBRARIES} ${PCSC_LIBRARY})
endif (UNIX)
endif (NOT APPLE)

target_compile_definitions(${APP_NAME} PUBLIC "APP_NAME=\"${APP_NAME}\"")

endforeach()

#===============================================================================

#set(CMAKE_INSTALL_BINDIR "local/bin")
set(DATAROOT ".")

foreach(APP_NAME ${MY_TARGETS}) 
MESSAGE(STATUS "Install ${APP_NAME} to ${CMAKE_INSTALL_PREFIX}")
#MESSAGE(STATUS "CMAKE_INSTALL_LOCALEDIR: ${CMAKE_INSTALL_LOCALEDIR}")
#MESSAGE(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
#MESSAGE(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
#MESSAGE(STATUS "CPACK_SET_DESTDIR: ${CPACK_SET_DESTDIR}")

# === macOS ===
if (APPLE)
install(TARGETS ${APP_NAME}
    BUNDLE DESTINATION .
    COMPONENT ${APP_NAME})
endif (APPLE)

# === Linux ===
if (NOT APPLE)
if (UNIX)
# ExecutablePath
install(TARGETS ${APP_NAME} RUNTIME DESTINATION local/bin COMPONENT ${APP_NAME})

# LocalizedResourcesDir
install(DIRECTORY lang/
	DESTINATION local/share/locale
	COMPONENT ${APP_NAME}
	FILES_MATCHING PATTERN "*.mo")

# ResourcesDir
install(FILES
    ${SRCS_CSS}
    ${SRCS_JS}
    DESTINATION local/share/${APP_NAME} 
    COMPONENT ${APP_NAME})

install(FILES
    "res/lin/${APP_NAME}.png"
    DESTINATION share/icons 
    COMPONENT ${APP_NAME})

install(FILES
    "${CMAKE_SOURCE_DIR}/res/lin/${APP_NAME}.desktop"
	DESTINATION share/applications
	COMPONENT ${APP_NAME})
endif (UNIX)
endif (NOT APPLE)

endforeach()

#===============================================================================
# Create a template configuration file.
# Later we'll use sed to replace the placeholder

foreach(APP_NAME placeholder) 
	MESSAGE("=== Packaging: <${APP_NAME}>")
	set (CPACK_PACKAGE_NAME ${APP_NAME})
	set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/res/lin/${APP_NAME}.png")
	set (CPACK_PACKAGE_VENDOR "ywesee")
	set (CPACK_PACKAGE_CONTACT "Zeno Davatz <zdavatz@ywesee.com>")
	set (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_DESCRIPTION})
	set (CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
	set (CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
	set (CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
	set (CPACK_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
	set (CPACK_PACKAGE_CHECKSUM "MD5")
	#set (CPACK_PACKAGE_CHECKSUM "SHA1")
	
	if (APPLE)
		MESSAGE("macOS installer")
		set(CPACK_GENERATOR "DragNDrop")
	else (APPLE)
	if (UNIX)
		MESSAGE("Linux installer")
		set(CPACK_GENERATOR "DEB;TGZ;RPM")
		set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT}) #required
		set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
		#set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
		#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libwwxgtk ( >= 3.1.4 ), libwebkit2gtk-4.0, pcscd, libssl, libgtk-3") # Specify dependencies here
		set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
	endif (UNIX)
	endif (APPLE)
	
	# https://cmake.org/cmake/help/latest/module/CPack.html
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
	set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
	#set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/welcome.txt")

endforeach()

# Must be after the last CPACK macros
INCLUDE(CPack)
