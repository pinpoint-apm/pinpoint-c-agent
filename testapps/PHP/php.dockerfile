FROM yiisoftware/yii2-php:7.4-fpm-nginx


# build ext
COPY config.m4 /pinpoint-c-agent/config.m4 
COPY pinpoint_php.cpp /pinpoint-c-agent/pinpoint_php.cpp 
COPY php_pinpoint_php.h /pinpoint-c-agent/php_pinpoint_php.h
COPY common /pinpoint-c-agent/common
COPY tests /pinpoint-c-agent/tests

RUN cd /pinpoint-c-agent/ && phpize && ./configure && make && make install
COPY testapps/PHP/php.ini   /usr/local/etc/php/conf.d/pinpoint-php-ext.ini

RUN composer create-project --prefer-dist yiisoft/yii2-app-basic /app
RUN composer require -w pinpoint-apm/pinpoint-php-aop:v3.0.2
RUN chown -R www-data:www-data /app
# Copy index.php
COPY testapps/PHP/index.php /app/web/index.php

EXPOSE 80