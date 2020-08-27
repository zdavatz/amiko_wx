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

eval SRC=$CONFIG_SRC_DIR
SRC_JSON=$(pwd)/json
SRC_SQLITE=$SRC/$SQLITE
SRC_WXWIDGETS=$SRC/$WXWIDGETS
SRC_APP=$(pwd)

eval BLD=$CONFIG_BLD_DIR/$APP
BLD_JSON=$BLD/json
BLD_WXWIDGETS=$BLD/$WXWIDGETS-$IDE
BLD_APP=$BLD/$APP-$IDE

eval BIN=$CONFIG_BIN_DIR/$APP
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
  if [ $BUILD_TYPE == "Debug" ]; then
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
cmake -G"$GENERATOR" \
	-D JSON_BuildTests=OFF \
	-D CMAKE_INSTALL_PREFIX=$BIN_JSON \
	$SRC_JSON
fi

if [ $STEP_BUILD_JSON ] ; then
	cd $BLD_JSON
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
    $SRC_APP
fi

if [ $STEP_COMPILE_APP ] && [ $CONFIG_GENERATOR_MK ] ; then
cd $BLD_APP
echo "=== Build"
make $MAKE_FLAGS
echo "=== Install into $BIN_APP"
make install
fi
