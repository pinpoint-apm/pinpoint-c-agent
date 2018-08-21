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

# SOURCE="${BASH_SOURCE[0]}"
# while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
#   DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#   SOURCE="$(readlink "$SOURCE")"
#   [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
# done
# DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

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



func_build_thrift(){
    if [ ! -f ${TP_PREFIX}/lib/libthrift.a ]; then
        cd ${TP_DIR}/thrift
        ./bootstrap.sh
        ./configure --prefix=$TP_PREFIX \
                    --with-cpp \
                    --with-boost=${TP_PREFIX} \
                    --with-boost-libdir=${TP_PREFIX} \
                    --with-php=no \
                    --with-python=no \
                    --with-ruby=no \
                    --with-nodejs=no \
                    --with-qt4=no \
                    --enable-tests=no  \
                    --with-java=no \
                    --with-qt5=no 


        make -j4 && make install
    fi
}


func_build_boost(){
    if [ ! -f ${TP_PREFIX}/lib/libboost_atomic.a ]; then
        cd ${TP_DIR}/boost
        ./bootstrap.sh
        ./b2 -j4 install --prefix=${TP_PREFIX}
        
    fi
}



deploy_third_library(){
    

    func_build_boost
    func_build_thrift
    
}

deploy_third_library ${TP_DIR}
