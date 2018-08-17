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

# for debug
#set -x
set -e

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

source ${DIR}/env.sh


func_fast_deploy_library(){
    echo "install automake bison flex g++  libtool make pkg-config [Superuser privilege]"
    if type "apt-get" >/dev/null; then
        # sudo apt-get innstall libboost-all-dev
        # ubuntu : boost is installed in /usr/include/boost
        sudo apt-get install automake bison flex g++ git libtool make pkg-config
    elif type  "yum" >/dev/null; then
        sudo yum install automake libtool flex bison pkgconfig gcc-c++
    else
        echo "not supported"
        exit 1
    fi
}

func_download_thrift(){
    if [ ! -f ${TP_DIR}/thrift-${THRIFT_VERSION}.tar.gz ];then
        cd ${TP_DIR}
        wget ${THRIFT_LINK}
    fi
}

func_build_thrift(){
    if [ ! -f ${TP_PREFIX}/lib/libthrift.a ]; then
        cd ${TP_DIR}
        tar zxvf thrift-${THRIFT_VERSION}.tar.gz
        cd thrift-${THRIFT_VERSION}
        ./configure --prefix=$TP_PREFIX \
                    --with-cpp \
                    --with-php=no \
                    --with-python=no \
                    --with-ruby=no \
                    --with-nodejs=no \
                    --with-qt4=no \
                    --enable-tests=no  \
                    --with-java=no \
                    --with-boost=${TP_PREFIX}

        make  && make install
        test 0 -ne $? && exit 0
    fi
}


func_download_boost(){
    if [ ! -f ${TP_DIR}/boost_${BOOST_VERSION_TAR}.tar.gz ];then
        cd ${TP_DIR}
        wget ${BOOST_LINK}
    fi
}

func_build_boost(){
    if [ ! -f ${TP_PREFIX}/lib/libboost_atomic.a ]; then
        cd ${TP_DIR}
        tar zxvf boost_${BOOST_VERSION_TAR}.tar.gz
        cd boost_${BOOST_VERSION_TAR}
        ./bootstrap.sh
        ./b2 install --prefix=${TP_PREFIX}
    fi
}

func_output_env(){

    echo "--------------------------------------------------------------------"
    echo "Adding below into your system"
    echo "export WITH_BOOST_PATH=/usr/local/"
    echo "export LD_LIBRARY_PATH=$1/lib:\$LD_LIBRARY_PATH"
    echo "--------------------------------------------------------------------"
    echo "NOTE: Keeping $1/lib in your LD_LIBRARY_PATH when you run pinpoint_php_agent"
    echo "--------------------------------------------------------------------"
}

deploy_third_library(){
    
    func_download_boost
    func_download_thrift

    func_fast_deploy_library

    func_build_boost
    func_build_thrift

    #func_fast_deploy_library
    #cd $1 && func_output_env ${TP_PREFIX}
}

deploy_third_library ${TP_DIR}
