#!/usr/bin/env bash

# for debug
#set -x

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

PROJECT_DIR="$( cd -P "$( dirname "$DIR" )" && pwd )"

PINPOINT_PHP_DIR=${PROJECT_DIR}/pinpoint_php
PINPOINT_COMMON_DIR=${PROJECT_DIR}/pinpoint_common

# third party
TP_DIR=${PROJECT_DIR}/thirdparty
TP_PREFIX=${TP_DIR}/installed

# https://issues.apache.org/jira/browse/THRIFT-3991
#THRIFT_VERSION=0.10.0
THRIFT_VERSION=0.11.0
THRIFT_DIR=${TP_DIR}/thrift-$THRIFT_VERSION
THRIFT_LINK="http://apache.fayea.com/thrift/${THRIFT_VERSION}/thrift-${THRIFT_VERSION}.tar.gz"

BOOST_VERSION=1.63.0
BOOST_VERSION_TAR=1_63_0
BOOST_DIR=${TP_DIR}/boost-boost-$1.63.0
BOOST_LINK="https://jaist.dl.sourceforge.net/project/boost/boost/${BOOST_VERSION}/boost_${BOOST_VERSION_TAR}.tar.gz"
