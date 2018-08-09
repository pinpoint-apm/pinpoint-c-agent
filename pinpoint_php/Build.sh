#!/bin/bash


WORKDIR=$PWD
SHOW_HELP=NO
JUST_CLEAN=NO
WITH_DEBUG=NO
MAKE_PARAS=""   # interface for make params
WITH_GCOV=NO
WITH_CI=NO
RELEASE=NO

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
        --enable-ci)                         WITH_CI=YES ;;
        --enable-gcov)                       WITH_GCOV=YES ;;
        clean)                               JUST_CLEAN=YES ;;
        --help)                              SHOW_HELP=YES ;;
        --always-make)                       MAKE_PARAS="-B" ;;
        -h)                                  SHOW_HELP=YES ;;
        --enable-release)                    RELEASE=YES ;;
       *)
            echo "$0: error: invalid option \"$option\""
            exit 1
        ;;
    esac
done



function init_env(){

    export EXT_LIBRRAYPATH="/user/local/lib"
    export DISABLE_64BIT=NO
    export DEBUG_FLAG=$WITH_DEBUG

    ## test boost and thrift
    export BOOST_PATH=$WITH_BOOST_PATH
    export THRIFT_PATH=$WITH_THRIFT_PATH
    echo "built common library ..."

    if [ -z "$JENKINS_DEFINE_CONFIG" ];then
        export CPUNUM=`cat /proc/cpuinfo |grep "processor"|wc -l`
    else
        export CPUNUM=$DISTCC_NODE_COUNT
    fi

}


function read_cmd(){

    if [ $SHOW_HELP = YES ] ; then
        echo "--help  "
        echo "--with-boost-path= "
        echo "--with-thrift-path= "
        echo "--enable-debug "
        echo "--always-make"
        echo "--enable-ci"
        echo "--enable-release"
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
    elif [ $WITH_CI = YES ] ;then
        export PINPOINT_CXXFLAG='-DTEST_SIMULATE'
        export PINPOINT_CFLAG='-DTEST_SIMULATE'
    else
        echo "none gcov "
        echo "none with ci"
    fi
}

function build_common(){

    cd ../pinpoint_common/ && make all -j$CPUNUM $JENKINS_DEFINE_CONFIG  $MAKE_PARAS 

    if [ $? != 0 ]; then
        echo "built common library failed ..."
        cd $WORKDIR
        exit -1
    fi

}

function build_agent(){
    echo "build pinpoint_php"
    cd $WORKDIR && phpize
    make clean >/dev/zero || echo "clean last building"

    if [ $WITH_GCOV = YES ] ; then
        ./configure  ${JENKINS_DEFINE_CONFIG} CFLAGS="-Wall $PINPOINT_CFLAG" CXXFLAGS="-Wall  -g3 $PINPOINT_CXXFLAG" --with-thrift-dir=$THRIFT_PATH/include --with-boost-dir=$BOOST_PATH/include   --enable-gcov 
    elif [ $RELEASE = YES ]; then
        ./configure ${JENKINS_DEFINE_CONFIG} CFLAGS="-Wall $PINPOINT_CFLAG" CXXFLAGS="-Wall $PINPOINT_CXXFLAG" --with-thrift-dir=$THRIFT_PATH/include  --with-boost-dir=$BOOST_PATH/include --enable-release
    else
        ./configure ${JENKINS_DEFINE_CONFIG} CFLAGS="-Wall $PINPOINT_CFLAG" CXXFLAGS="-Wall $PINPOINT_CXXFLAG" --with-thrift-dir=$THRIFT_PATH/include --with-boost-dir=$BOOST_PATH/include  
    fi

    make  $MAKE_PARAS -j$CPUNUM
}

function main(){

    init_env 
    read_cmd
    build_common
    build_agent    

}

main 
