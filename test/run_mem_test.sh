#!/bin/sh
valgrind --track-fds=yes --time-stamp=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind.log ./$1