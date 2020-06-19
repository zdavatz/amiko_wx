#!/bin/bash
# Alex Bettarini - 12 Jun 2020
# ywesee

APP=AmiKo-wx

for f in seed steps
do
if [ ! -f $f.conf ] ; then
    cp $f.in.conf $f.conf
    echo "Please edit $f.conf to suite your preferences"
    exit 0
fi
done

source steps.conf
source seed.conf

WXWIDGETS_VERSION=3.1.3

if [ $CONFIG_GENERATOR_XC ] ; then
    GENERATOR="Xcode"
    IDE=xc
elif [ $CONFIG_GENERATOR_MK ] ; then
    GENERATOR="Unix Makefiles"
    IDE=mk
elif [ $CONFIG_GENERATOR_CL ] ; then
    GENERATOR="CodeLite - Unix Makefiles"
    IDE=cl
elif [ $CONFIG_GENERATOR_CB ] ; then
    GENERATOR="CodeBlocks - Unix Makefiles"
    IDE=cb
fi

WXWIDGETS=wxWidgets-$WXWIDGETS_VERSION

eval SRC=$CONFIG_SRC_DIR
SRC_WXWIDGETS=$SRC/$WXWIDGETS
SRC_APP=$(pwd)

eval BLD=$CONFIG_BLD_DIR/$APP
BLD_WXWIDGETS=$BLD/$WXWIDGETS
BLD_APP=$BLD/$APP-$IDE

eval BIN=$CONFIG_BIN_DIR/$APP
BIN_WXWIDGETS=$BIN/$WXWIDGETS
BIN_APP=$BIN/$APP

echo "SRC: $SRC"
echo "BLD: $BLD"
echo "BIN: $BIN"

if [[ $(uname -s) == "Darwin" ]] ; then
NCPU=$(sysctl -n hw.ncpu)
WX_OPTIONS_OS_AUTOTOOLS="--with-cocoa --with-mac"
WX_OPTIONS_OS_CMAKE=
elif [[ $(uname -s) == "Linux" ]] ; then
NCPU=$(nproc --all)
WX_OPTIONS_OS_AUTOTOOLS="--with-gtk=3"
WX_OPTIONS_OS_CMAKE=" -D wxBUILD_USE_STATIC_RUNTIME=ON -D wxBUILD_SAMPLES=ALL"
else
NCPU=$NUMBER_OF_PROCESSORS
WX_OPTIONS_OS_AUTOTOOLS="--with-msw"
WX_OPTIONS_OS_CMAKE="--with-msw -D wxBUILD_USE_STATIC_RUNTIME=ON"
fi

MAKE_FLAGS="-j $NCPU VERBOSE=1"
CMAKE=cmake
mkdir -p $SRC

#-------------------------------------------------------------------------------
if [ $STEP_DOWNLOAD_SOURCES_WXWIDGETS ] ; then
if [ -d $SRC_WXWIDGETS ] ; then
echo "=== Download $WXWIDGETS exists"
else
cd $SRC
echo "=== Download $WXWIDGETS"
    wget https://github.com/wxWidgets/wxWidgets/releases/download/v$WXWIDGETS_VERSION/wxWidgets-$WXWIDGETS_VERSION.tar.bz2
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
    -D CMAKE_BUILD_TYPE=Release \
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
#COMPILER_FLAGS="$COMPILER_FLAGS -DGL_SILENCE_DEPRECATION=1 -DWITH_OGL_CORE"
#COMPILER_FLAGS="$COMPILER_FLAGS -DGL_SILENCE_DEPRECATION=1"
$CMAKE -G"$GENERATOR" \
    -D CMAKE_INSTALL_PREFIX=$BIN_APP \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_CXX_FLAGS="$COMPILER_FLAGS" \
    -D WX_ROOT=$BIN_WXWIDGETS \
    $SRC_APP
fi

if [ $STEP_COMPILE_APP ] && [ $CONFIG_GENERATOR_MK ] ; then
cd $BLD_APP
echo "=== Build $APP_NAME"
make $MAKE_FLAGS
echo "=== Install $APP_NAME"
make install
fi
