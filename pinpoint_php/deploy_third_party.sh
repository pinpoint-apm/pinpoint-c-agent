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


THRIFT_LINK="http://apache.fayea.com/thrift/0.10.0/thrift-0.10.0.tar.gz"

ROOT=$PWD

func_fast_deploy_library(){
    echo "install automake bison flex g++  libboost-all-dev  libtool make pkg-config [Superuser privilege]"
    if type "apt-get" >/dev/null; then
        sudo apt-get install automake bison flex g++ git libboost-all-dev  libtool make pkg-config
    elif type  "yum" >/dev/null; then
        sudo yum install automake libtool flex bison pkgconfig gcc-c++ boost-devel
    else
        echo "not supported"
        exit 1
    fi 

}

func_download_third_packets(){
    cd $1
    if [ ! -f thrift-0.10.0.tar.gz ];then
        wget $THRIFT_LINK 
    fi
}

func_deploy_thrift(){
    tar zxvf thrift-0.10.0.tar.gz
    cd thrift-0.10.0
    ./configure --prefix=$1 \
                --with-cpp \
                --with-php=no \
                --with-python=no \
                --with-ruby=no \
                --with-nodejs=no \
                --with-qt4=no \
                --enable-tests=no  \
                --with-java=no 

    make  && make install
    test 0 -ne $? && exit 0
}

func_output_env(){

    echo "--------------------------------------------------------------------"
    echo "Adding below into your system"
    echo "export WITH_BOOST_PATH=/usr/local/"
    echo "export WITH_THRIFT_PATH=$1"
    echo "export LD_LIBRARY_PATH=$1/lib:\$LD_LIBRARY_PATH"
    echo "--------------------------------------------------------------------"
    echo "NOTE: Keeping $1/lib in your LD_LIBRARY_PATH when you run pinpoint_php_agent"
    echo "--------------------------------------------------------------------"
}

deploy_third_library(){
    cd $1 && func_download_third_packets $1 
    func_fast_deploy_library
    cd $1 && func_deploy_thrift $1
    cd $1 && func_output_env $1
}

deploy_third_library $1