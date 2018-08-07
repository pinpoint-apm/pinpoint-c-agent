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

BOOST_LINK="https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz"
THRIFT_LINK="http://apache.fayea.com/thrift/0.10.0/thrift-0.10.0.tar.gz"

ROOT=$PWD

abort(){
    echo "error " $? $BASH_COMMAND  ${BASH_LINENO[0]}
}

func_download_third_packets(){
    if [ ! -f boost_1_64_0.tar.gz ];then
        wget $BOOST_LINK 
    fi

    if [ ! -f thrift-0.10.0.tar.gz ];then
        wget $THRIFT_LINK 
    fi
}

trap 'abort' ERR

func_deploy_boost(){
    tar zxvf boost_1_64_0.tar.gz
    cd boost_1_64_0
    ./bootstrap.sh --prefix=$1
    ./b2 -j4 install 
    test 0 -ne $? && exit 0
}

func_deploy_thrift(){
    tar zxvf thrift-0.10.0.tar.gz
    cd thrift-0.10.0
    ./configure --prefix=$1 \
                --with-boost=$1 \
                --with-boost-libdir=$1 \
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

deploy_main(){
    cd $1
    func_download_third_packets 
    cd $1
    func_deploy_boost $1
    cd $1
    func_deploy_thrift $1
    cd $ROOT
}

deploy_main $1