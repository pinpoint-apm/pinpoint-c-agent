#!/bin/bash

PINPOINT_PHP_VERSION=v0.1.11
PINPOINT_PHP_VERSION_MD5SUM=
func_check_command(){
    command -v $1 >/dev/null 2>&1 ||  { echo >&2 " require $1 command. Aborting . "; exit 1; } 
}


func_output_w(){
    RED='\033[0;31m'
    NC='\033[0m'
    echo "${RED} $1 ${NC}"
}

func_output_n(){
    GREEN='\033[0;32m'
    NC='\033[0m'
    echo "${RED} $1 ${NC}"
}

func_download_extension(){
    mkdir -p /tmp/pinpoint_php &&  cd /tmp/pinpoint_php && curl -L -o pinpoint_php.tar.gz https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.6.0/pinpoint_php@v0.6.0-418bec8f2f0ff1d9213f683214f1b835-2024-07-08.tar.gz &&  tar xvf pinpoint_php.tar.gz  && phpize && ./configure && make install

    # mkdir -p /tmp/pinpoint_php &&  cd /tmp/pinpoint_php && curl -L -o pinpoint_php.tar.gz https://github.com/eeliu/pinpoint-c-agent/releases/download/$PINPOINT_PHP_VERSION/pinpoint_php@$PINPOINT_PHP_VERSION.tar.gz &&  tar  xvf pinpoint_php.tar.gz  && phpize && ./configure && make install
    #  && rm /tmp/pinpoint_php* -rf
}

func_install_pinpoint_config(){
    echo "enable pinpoint_php into php(php.ini);"
    cat << EOF >>/tmp/pinpoint_php.ini
[pinpoint_php]
extension=pinpoint_php.so
# Collector-agent's TCP address, ip,port:Collector-Agent's ip,port
pinpoint_php.CollectorHost=tcp:dev-collector:10000
# 0 is recommanded
pinpoint_php.SendSpanTimeOutMs=0
# request should be captured duing 1 second. < 0 means no limited
pinpoint_php.TraceLimit=-1 
# DEBUG the agent 
# error_reporting = E_ALL
# log_errors = On
# should be set false if in production env
pinpoint_php.DebugReport=false
EOF
    INI_DIR=`php-config --ini-dir`
    if [ -d "$INI_DIR" ]; then
        cp /tmp/pinpoint_php.ini $INI_DIR
        echo "install pinpoint_php into $INI_DIR";
        echo "<<< $INI_DIR/pinpoint_php.ini >>>";
    else
        func_output_w "Your php does not set --ini-dir, enable pinpoint_php into php.ini !!!"
        func_output_w ">> php.ini"
        cat /tmp/pinpoint_php.ini
        func_output_w ">> EOF"
    fi
    rm /tmp/pinpoint_php.ini
}

func_show_pinpoint_php(){
    php -r "echo 'TEST: installed pinpoint_php:'. phpversion('pinpoint_php');"
    func_output_n " \n everything looks done !";
}

main(){
    for cmd in php phpize gcc make php-config curl; do
        func_check_command $cmd
    done 
    func_download_extension 
    func_install_pinpoint_config
    func_show_pinpoint_php
}

main 
