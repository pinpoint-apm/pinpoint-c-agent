FROM mondedie/flarum:latest

RUN apk update && apk add php8-dev build-base

# COPY config.m4 /pinpoint-c-agent/config.m4 
# COPY pinpoint_php.cpp /pinpoint-c-agent/pinpoint_php.cpp 
# COPY php_pinpoint_php.h /pinpoint-c-agent/php_pinpoint_php.h
# COPY common /pinpoint-c-agent/common
# COPY testapps/php_wordpress/pinpoint_php.ini /pinpoint-c-agent/pinpoint_php.ini

# RUN cd /pinpoint-c-agent/ && phpize && ./configure && make install
# RUN INI_DIR=`php-config --ini-dir` && cp /pinpoint-c-agent/pinpoint_php.ini $INI_DIR

RUN curl -sL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/latest/download/install_pinpoint_php.sh | sh

COPY testapps/flarum/site.php /flarum/app/site.php
RUN cd /flarum/app/ && composer require -w pinpoint-apm/pinpoint-php-aop