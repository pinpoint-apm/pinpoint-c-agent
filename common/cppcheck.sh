#!/bin/sh

cppcheck --language=c++ --std=c++11 --suppress=*:*googletest-src/* --suppress=*:*jsoncpp/* --suppress=*:*.cc --project=./compile_commands.json --enable=all -j10  2>cppcheck.log