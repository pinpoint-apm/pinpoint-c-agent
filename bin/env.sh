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
TP_PREFIX=${TP_DIR}/var

THRIFT_DIR=${TP_DIR}/thrift

BOOST_DIR=${TP_DIR}/boost

