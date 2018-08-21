#!/usr/bin/env bash
# Copyright 2018 NAVER Corp.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#for debug
set -e
set -x

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"


source $DIR/../bin/env.sh

WITH_THRIFT_PATH=$TP_PREFIX
WITH_BOOST_PATH=$TP_PREFIX

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
        --enable-debug)                      WITH_DEBUG=YES ;;
        --enable-ci)                         WITH_CI=YES ;;
        --enable-gcov)                       WITH_GCOV=YES ;;
        --with-boost=*)                      USER_BOOST_PATH="$value" ;;
        --with-thrift=*)                     USER_THRIFT_PATH="$value" ;;
        clean)                               JUST_CLEAN=YES ;;
        --help)                              SHOW_HELP=YES ;;
        --always-make)                       MAKE_PARAS="-B" ;;
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

    export DISABLE_64BIT=NO
    export DEBUG_FLAG=${WITH_DEBUG}

    if [[ -d $USER_BOOST_PATH/include/boost && -d $USER_THRIFT_PATH/include/thrfit ]]
    then
        export BOOST_PATH=${USER_BOOST_PATH}
        export THRIFT_PATH=${USER_THRIFT_PATH}
    elif [[ -d $WITH_BOOST_PATH/include/boost  && -d $WITH_THRIFT_PATH/include/thrfit ]]
    then  
        echo "WITH_BOOST_PATH="$WITH_BOOST_PATH
        echo "WITH_THRIFT_PATH="$WITH_THRIFT_PATH
        export BOOST_PATH=${WITH_BOOST_PATH}
        export THRIFT_PATH=${WITH_THRIFT_PATH}
    else
        echo "Install boost and thrift into " ${TP_PREFIX}
        source deploy_third_party.sh ${TP_PREFIX}
        export BOOST_PATH=${TP_PREFIX}
        export THRIFT_PATH=${TP_PREFIX}
    fi

    
    echo "built common library ..."

    if [ -z "$JENKINS_DEFINE_CONFIG" ];then
        export CPUNUM=`cat /proc/cpuinfo |grep "processor"|wc -l`
    else
        export CPUNUM=$DISTCC_NODE_COUNT
    fi

    ## check the phpize
    phpize -v || (echo "error: phpize not find exit 1" && exit 1)

}


function read_cmd(){

    if [ $SHOW_HELP = YES ] ; then
        echo "--help"
        echo "--enable-debug"
        echo "--always-make"
        echo "--enable-ci"
        echo "--enable-release"
        exit 0
    fi

    ## clean
    if [ $JUST_CLEAN = YES ] ; then
        cd ${PINPOINT_COMMON_DIR} && make clean
        cd ${PINPOINT_PHP_DIR} && phpize --clean
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
    fi
}

function build_common(){

    cd ${PINPOINT_COMMON_DIR} && make all -j$CPUNUM $JENKINS_DEFINE_CONFIG  $MAKE_PARAS

    if [ $? != 0 ]; then
        echo "built common library failed ..."
        cd ${PINPOINT_PHP_DIR}
        exit -1
    fi

}

function build_agent(){
    echo "build pinpoint_php"
    cd ${PINPOINT_PHP_DIR} && phpize
    make clean >/dev/zero || echo "clean last building"

    if [ $WITH_GCOV = YES ] ; then
        ./configure  ${JENKINS_DEFINE_CONFIG} CFLAGS="-Wall $PINPOINT_CFLAG" CXXFLAGS="-Wall  -g3 $PINPOINT_CXXFLAG" --with-thrift-dir=$THRIFT_PATH/include --with-boost-dir=$BOOST_PATH/include   --enable-gcov 
    elif [ $RELEASE = YES ]; then
        ./configure ${JENKINS_DEFINE_CONFIG} CFLAGS="-Wall $PINPOINT_CFLAG" CXXFLAGS="pinpoint_php-Wall $PINPOINT_CXXFLAG" --with-thrift-dir=$THRIFT_PATH/include  --with-boost-dir=$BOOST_PATH/include --enable-release
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
