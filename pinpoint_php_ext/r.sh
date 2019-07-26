#!/bin/sh
make 
make install
pkill php-fpm
sleep 1
php-fpm