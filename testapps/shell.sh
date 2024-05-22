#!/bin/bash
export EXT_PATH=$(php-config --extension-dir)
ls -al ${EXT_PATH}/ 
cp ${EXT_PATH}/*.so modules/
make test -m