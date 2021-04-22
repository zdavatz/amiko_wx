#!/bin/bash
# Alex Bettarini - 12 Jun 2020
# ywesee

APP=AmiKo-wx

for f in seed steps
do
if [ ! -f $f.conf ] ; then
    cp $f.in.conf $f.conf
    echo "Please edit $f.conf to suit your preferences"
    exit 0
fi
done

source steps.conf
source seed.conf

WXWIDGETS_VERSION=3.1.4
SQLITE_VERSION=3320300
CURL_VERSION=7.73.0

if [ $CONFIG_GENERATOR_XC ] ; then
    GENERATOR="Xcode"
    IDE=xc
elif [ $CONFIG_GENERATOR_MK ] ; then
    GENERATOR="Unix Makefiles"
    IDE=mk
elif [ $CONFIG_GENERATOR_EC ] ; then
    GENERATOR="Eclipse CDT4 - Unix Makefiles"
    IDE=ec
elif [ $CONFIG_GENERATOR_CL ] ; then
    GENERATOR="CodeLite - Unix Makefiles"
    IDE=cl
elif [ $CONFIG_GENERATOR_CB ] ; then
    GENERATOR="CodeBlocks - Unix Makefiles"
    IDE=cb
fi

WXWIDGETS=wxWidgets-$WXWIDGETS_VERSION
SQLITE=sqlite-amalgamation-$SQLITE_VERSION
CURL=curl-$CURL_VERSION
TARGET_LIST="AmiKo CoMed"
LANGUAGE_LIST="de_CH fr_CH"
WD=$(pwd)

eval SRC=$CONFIG_SRC_DIR
SRC_JSON=$WD/json
SRC_CURL=$SRC/$CURL
SRC_SQLITE=$SRC/$SQLITE
SRC_WXWIDGETS=$SRC/$WXWIDGETS
SRC_APP=$WD

eval BLD=$CONFIG_BLD_DIR/$APP
BLD_CURL=$BLD/curl
BLD_JSON=$BLD/json
BLD_WXWIDGETS=$BLD/$WXWIDGETS-$IDE
BLD_APP=$BLD/$APP-$IDE

eval BIN=$CONFIG_BIN_DIR/$APP
BIN_CURL=$BIN/curl
BIN_JSON=$BIN/json
BIN_WXWIDGETS=$BIN/$WXWIDGETS
BIN_APP=$BIN # each of the two targets will be in its own subdirectory

echo "SRC: $SRC"
echo "BLD: $BLD"
echo "BIN: $BIN"

if [[ $(uname -s) == "Darwin" ]] ; then
NCPU=$(sysctl -n hw.ncpu)
WX_OPTIONS_OS_AUTOTOOLS="--with-cocoa --with-mac"
WX_OPTIONS_OS_CMAKE=
elif [[ $(uname -s) == "Linux" ]] ; then
NCPU=$(nproc --all)
  if [[ $BUILD_TYPE == "Debug" ]]; then
    WX_OPTIONS_OS_AUTOTOOLS_DEBUG="--enable-debug --enable-debug_gdb"
  fi
WX_OPTIONS_OS_AUTOTOOLS="--with-gtk=3 --with-cxx=17 --enable-utf8 --with-regex --enable-html --enable-webview ${WX_OPTIONS_OS_AUTOTOOLS_DEBUG}"
WX_OPTIONS_OS_CMAKE=" -D wxBUILD_USE_STATIC_RUNTIME=ON -D wxBUILD_SAMPLES=ALL"
else
NCPU=$NUMBER_OF_PROCESSORS
WX_OPTIONS_OS_AUTOTOOLS="--with-msw"
WX_OPTIONS_OS_CMAKE="--with-msw -D wxBUILD_USE_STATIC_RUNTIME=ON"
fi

if [ -z $BUILD_TYPE ]; then BUILD_TYPE=Release; fi
MAKE_FLAGS="-j $NCPU VERBOSE=1"
CMAKE=cmake
mkdir -p $SRC
$CMAKE --version | head -n 1

#-------------------------------------------------------------------------------
if [ $STEP_DOWNLOAD_SOURCES_SQLITE ] ; then
if [ -d $SRC_SQLITE ] ; then
    echo "=== Download $SQLITE exists"
else
    cd $SRC
    echo "=== Download $SQLITE"
    wget https://www.sqlite.org/2020/$SQLITE.zip
    unzip $SQLITE.zip
    rm $SQLITE.zip
fi

    # Remove symlink if it exists
    if [ -L $SRC_APP/sqlite ] ; then
        rm -f $SRC_APP/sqlite
    fi
    
    # Recreate symlink
    ln -s $SRC_SQLITE $SRC_APP/sqlite
fi

#------------------------------------------------------------------------------
if [ $STEP_CONFIGURE_JSON ] ; then
mkdir -p $BLD_JSON ; cd $BLD_JSON
echo "=== Configure JSON, install to $BIN_JSON"
$CMAKE -G"$GENERATOR" \
	-D JSON_BuildTests=OFF \
	-D CMAKE_INSTALL_PREFIX=$BIN_JSON \
	$SRC_JSON
fi

if [ $STEP_BUILD_JSON ] ; then
	cd $BLD_JSON
	make $MAKE_FLAGS
  make install
fi

#------------------------------------------------------------------------------

if [ $STEP_DOWNLOAD_SOURCES_CURL ] ; then
if [ -d $SRC_CURL ] ; then
echo "=== Download $SRC_CURL exists"
else
cd $SRC
echo "=== Download $SRC_CURL"
    wget https://curl.se/download/$CURL.tar.gz
    tar -xvzf $CURL.tar.gz
    rm $CURL.tar.gz
fi
fi

if [ $STEP_CONFIGURE_CURL ] ; then
mkdir -p $BLD_CURL ; cd $BLD_CURL
echo "=== Configure CURL, install to $BIN_CURL"
    if [[ $(uname -s) == "Darwin" ]] ; then
        export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig" 
        $SRC_CURL/configure --without-ssl --with-secure-transport --prefix=$BIN_CURL --without-librtmp --without-libidn --without-libidn2 --without-zstd --without-brotli
    else
      $SRC_CURL/configure --with-ssl --prefix=$BIN_CURL --without-librtmp --without-libidn --without-libidn2 --without-zstd --without-brotli
    fi
fi

if [ $STEP_BUILD_CURL ] ; then
    cd $BLD_CURL
    make $MAKE_FLAGS
  make install
fi

#-------------------------------------------------------------------------------
if [ $STEP_DOWNLOAD_SOURCES_WXWIDGETS ] ; then
if [ -d $SRC_WXWIDGETS ] ; then
echo "=== Download $WXWIDGETS exists"
else
cd $SRC
echo "=== Download $WXWIDGETS"
    wget https://github.com/wxWidgets/wxWidgets/releases/download/v$WXWIDGETS_VERSION/$WXWIDGETS.tar.bz2
    tar -xjf $WXWIDGETS.tar.bz2
    rm $WXWIDGETS.tar.bz2
fi
fi

if [ $STEP_INFO_WXWIDGETS ] ; then
mkdir -p $BLD_WXWIDGETS ; cd $BLD_WXWIDGETS
echo -e "=== Info ${WXWIDGETS}\n\t SRC: ${SRC_WXWIDGETS}\n\t BLD: ${BLD_WXWIDGETS}\n\t BIN: $BIN_WXWIDGETS"
$SRC_WXWIDGETS/configure --help > $WXWIDGETS--help.txt
fi

if [ $STEP_CONFIGURE_WXWIDGETS ] ; then
mkdir -p $BLD_WXWIDGETS ; cd $BLD_WXWIDGETS
echo "=== Configure $WXWIDGETS, install to $BIN_WXWIDGETS"
export CPPFLAGS=-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES
export CXXFLAGS=-D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES
if [[ $(uname -s) == "Linux" ]] ; then
$SRC_WXWIDGETS/configure --prefix=$BIN_WXWIDGETS \
    $WX_OPTIONS_OS_AUTOTOOLS \
    --enable-unicode --disable-shared --disable-sys-libs
else
rm -f CMakeCache.txt
$CMAKE -G"$GENERATOR" \
    -D CMAKE_INSTALL_PREFIX="$BIN_WXWIDGETS" \
    -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
    -D CMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    $WX_OPTIONS_OS_CMAKE \
    -D wxBUILD_SHARED=OFF \
    $SRC_WXWIDGETS

#    -D wxBUILD_SAMPLES=ALL \

fi
fi

if [ $STEP_COMPILE_WXWIDGETS ] ; then
echo "=== Build $WXWIDGETS"
cd $BLD_WXWIDGETS
make $MAKE_FLAGS
make install
fi

#-------------------------------------------------------------------------------
if [ $STEP_CONFIGURE_APP ] ; then
echo "=== Configure $APP_NAME in $BLD_APP"
mkdir -p $BLD_APP ; cd $BLD_APP
rm -f CMakeCache.txt
$CMAKE -G"$GENERATOR" \
    -D CMAKE_INSTALL_PREFIX=$BIN_APP \
    -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
    -D CMAKE_CXX_FLAGS="$COMPILER_FLAGS" \
    -D WX_ROOT=$BIN_WXWIDGETS \
    -D JSON_DIR=$BIN_JSON \
    -D CURL_DIR=$BIN_CURL \
    $SRC_APP

# Normally cpack creates a single config file for the whole project, resulting in a single DEB file
# We want two DEB files so we are doing some renaming. Idea from:
# https://cmake.cmake.narkive.com/vCbIAsrO/creating-multiple-deb-packages-from-one-source-tree-using-cpack
for f in $TARGET_LIST
do
  cp CPackConfig.cmake CPackConfig.$f.cmake
  sed -i -e "s/;ALL;/;${f};/g" CPackConfig.$f.cmake
  sed -i -e "s/placeholder/${f}/g" CPackConfig.$f.cmake
done

fi

#-------------------------------------------------------------------------------

if [ $STEP_COMPILE_APP ] && [ $CONFIG_GENERATOR_MK ] ; then
cd $BLD_APP
echo "=== Build"
make $MAKE_FLAGS
# Make binary load the embeded library
if [[ $(uname -s) == "Darwin" ]] ; then
mkdir ./AmiKo.app/Contents/Frameworks
mkdir ./CoMed.app/Contents/Frameworks
cp ${BIN_CURL}/lib/libcurl.4.dylib ./AmiKo.app/Contents/Frameworks/libcurl.4.dylib
cp ${BIN_CURL}/lib/libcurl.4.dylib ./CoMed.app/Contents/Frameworks/libcurl.4.dylib
echo "Patching binary"
install_name_tool -change ${BIN_CURL}/lib/libcurl.4.dylib @executable_path/../Frameworks/libcurl.4.dylib ./AmiKo.app/Contents/MacOS/AmiKo
install_name_tool -change ${BIN_CURL}/lib/libcurl.4.dylib @executable_path/../Frameworks/libcurl.4.dylib ./CoMed.app/Contents/MacOS/CoMed

LIBSSL_PATH=$(otool -L ./AmiKo.app/Contents/MacOS/AmiKo | grep libssl | cut -f 2 | cut -d ' ' -f 1)
LIBCRYPTO_PATH=$(otool -L ./AmiKo.app/Contents/MacOS/AmiKo | grep libcrypto | cut -f 2 | cut -d ' ' -f 1)
LIBNGHTTP2_PATH=$(otool -L ${BIN_CURL}/lib/libcurl.4.dylib | grep libnghttp2 | cut -f 2 | cut -d ' ' -f 1)
echo "LIBSSL_PATH: $LIBSSL_PATH"
echo "LIBCRYPTO_PATH: $LIBCRYPTO_PATH"
echo "LIBNGHTTP2_PATH: $LIBNGHTTP2_PATH"

cp ${LIBSSL_PATH} ./AmiKo.app/Contents/Frameworks/libssl.dylib
cp ${LIBSSL_PATH} ./CoMed.app/Contents/Frameworks/libssl.dylib
cp ${LIBCRYPTO_PATH} ./AmiKo.app/Contents/Frameworks/libcrypto.dylib
cp ${LIBCRYPTO_PATH} ./CoMed.app/Contents/Frameworks/libcrypto.dylib
cp ${LIBNGHTTP2_PATH} ./AmiKo.app/Contents/Frameworks/libnghttp2.dylib
cp ${LIBNGHTTP2_PATH} ./CoMed.app/Contents/Frameworks/libnghttp2.dylib

echo "Patching binary - SSL"
install_name_tool -change ${LIBSSL_PATH} @executable_path/../Frameworks/libssl.dylib ./AmiKo.app/Contents/MacOS/AmiKo
install_name_tool -change ${LIBSSL_PATH} @executable_path/../Frameworks/libssl.dylib ./CoMed.app/Contents/MacOS/CoMed
echo "Patching binary - Crypto"
install_name_tool -change ${LIBCRYPTO_PATH} @executable_path/../Frameworks/libcrypto.dylib ./AmiKo.app/Contents/MacOS/AmiKo
install_name_tool -change ${LIBCRYPTO_PATH} @executable_path/../Frameworks/libcrypto.dylib ./CoMed.app/Contents/MacOS/CoMed
echo "Patching CURL - NGHTTP2"
install_name_tool -change $(otool -L ./AmiKo.app/Contents/Frameworks/libcurl.4.dylib | grep libnghttp2 | cut -f 2 | cut -d ' ' -f 1) @executable_path/../Frameworks/libnghttp2.dylib ./AmiKo.app/Contents/Frameworks/libcurl.4.dylib
echo "Patching SSL - Crypto"
install_name_tool -change $(otool -L ./AmiKo.app/Contents/Frameworks/libssl.dylib | grep libcrypto | cut -f 2 | cut -d ' ' -f 1) @executable_path/../Frameworks/libcrypto.dylib ./AmiKo.app/Contents/Frameworks/libssl.dylib
fi
fi

#-------------------------------------------------------------------------------
# (Linux only) Install the whole project, not just a single target

if [ $STEP_INSTALL_APP ] && [[ $(uname -s) == "Linux" ]] ; then
cd $BLD_APP
echo "=== Install into $BIN_APP"

for f in $TARGET_LIST
do
  echo "--- target: $f"
  cp $BLD_APP/$f $BIN_APP/$f
  cp $SRC_APP/res/lin/$f.desktop $(xdg-user-dir DESKTOP)
  sudo cp $SRC_APP/res/lin/$f.png /usr/share/icons/$f.png

  mkdir -p $HOME/.$f

  USER_DATA_DIR="/usr/local/share/$f"
  cp $SRC_APP/res/*.css $USER_DATA_DIR/
  cp $SRC_APP/res/*.js  $USER_DATA_DIR/
done

for LANG in $LANGUAGE_LIST
do
  echo "--- language: $LANG"
  LOCALIZED_RESOURCES_DIR="/usr/local/share/locale/${LANG}/LC_MESSAGES"
  sudo mkdir -p $LOCALIZED_RESOURCES_DIR
  sudo cp $SRC_APP/lang/$LANG/LC_MESSAGES/amiko.mo ${LOCALIZED_RESOURCES_DIR}/
done

fi

#-------------------------------------------------------------------------------
if [ $STEP_CREATE_INSTALLER ] ; then
cd $BLD_APP

for f in $TARGET_LIST
do
  echo "=== Create installer $f $BUILD_TYPE"
  cpack -C $BUILD_TYPE \
	-D CMAKE_INSTALL_BINDIR="/usr/local/bin" \
	-D DATAROOT="/usr/share" \
	--config CPackConfig.$f.cmake
done
fi
