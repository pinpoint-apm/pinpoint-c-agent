#!/usr/bin/env bash
## sample
## Build.sh  --with-boost=... --with-thrift= --enable-debug --disable-64bit

set -x

WORKDIR=$PWD/../
SHOW_HELP=NO
JUST_CLEAN=NO
WITH_DEBUG=NO
MAKE_PARAS=""   # interface for make params
WITH_GCOV=NO

for option
do
    opt="$opt `echo $option | sed -e \"s/\(--[^=]*=\)\(.* .*\)/\1'\2'/\"`"

    case "$option" in
        -*=*) value=`echo "$option" | sed -e 's/[-_a-zA-Z0-9]*=//'` ;;
           *) value="" ;;

    esac
    case "$option" in

        --with-boost-path=*)                 WITH_BOOST_PATH="$value"           ;;
        --with-thrift-path=*)                WITH_THRIFT_PATH="$value" ;;
        --enable-debug)                      WITH_DEBUG=YES ;;
        --enable-gcov)					     WITH_GCOV=YES ;;
        clean)                               JUST_CLEAN=YES ;;
        --help)                              SHOW_HELP=YES ;;
        --always-make)                       MAKE_PARAS="-B" ;;
        -h)                                  SHOW_HELP=YES ;;
#        --osplate)                DISABLE_64BIT=YES ;;
       *)
            echo "$0: error: invalid option \"$option\""
            exit 1
        ;;
    esac
done

if [ $SHOW_HELP = YES ] ; then
    echo "--help  "
    echo "--with-boost-path= "
    echo "--with-thrift-path= "
    echo "--enable-debug "
    echo "--always-make "
    exit 0
fi

## clean
if [ $JUST_CLEAN = YES ] ; then
    cd ../pinpoint_common/ && make clean
    cd ../pinpoint_php/ && phpize --clean
    exit 0
fi

if [ $WITH_GCOV = YES ] ; then
	## export GLOBAL CXXFLAGS
	WITH_DEBUG=YES
	export PINPOINT_CXXFLAG='-fprofile-arcs -ftest-coverage -DTEST_SIMULATE'
	export PINPOINT_CFLAG='-fprofile-arcs -ftest-coverage -DTEST_SIMULATE'
else
    export PINPOINT_CXXFLAG='-DTEST_SIMULATE'
    export PINPOINT_CFLAG='-DTEST_SIMULATE'    
    WITH_DEBUG=YES
fi

export EXT_LIBRRAYPATH="/user/local/lib"
export DISABLE_64BIT=NO
export DEBUG_FLAG=$WITH_DEBUG

## test boost and thrift
export BOOST_PATH=$WITH_BOOST_PATH
export THRIFT_PATH=$WITH_THRIFT_PATH
export CPUNUM=20

cd $WORKDIR
cd ../pinpoint_common/ && make all -j$CPUNUM $JENKINS_DEFINE_CONFIG  $MAKE_PARAS 

if [ $? != 0 ]; then
    echo "built common library failed ..."
    cd $WORKDIR
    exit -1
fi

echo "built pinpoint_php"
if [ $WITH_DEBUG = YES ] ; then
    cd $WORKDIR && phpize
    ./configure  ${JENKINS_DEFINE_CONFIG} CXXFLAGS="-Wall  -g3 $PINPOINT_CXXFLAG" --with-thrift-dir=$BOOST_PATH/include --with-boost-dir=$THRIFT_PATH/include   --enable-gcov 
    make  $MAKE_PARAS -j$CPUNUM
else
    cd $WORKDIR && phpize
    ./configure ${JENKINS_DEFINE_CONFIG} CFLAGS='-Wall' CXXFLAGS='-Wall' --with-thrift-dir=$BOOST_PATH/include --with-boost-dir=$THRIFT_PATH/include 
    make $MAKE_PARAS -j$CPUNUM
fi



