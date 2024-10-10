# FROM ubuntu:20.04
FROM php:7.4-fpm
WORKDIR /workspace
RUN DEBIAN_FRONTEND="noninteractive" apt update && apt-get install -y  libmemcached-dev zlib1g-dev libssl-dev  pkg-config librdkafka-dev libzip-dev zip 
COPY testapps/SimplePHP/php.ini /usr/local/etc/php/conf.d/
COPY testapps/SimplePHP/src /workspace/src
COPY testapps/SimplePHP/composer-real.json /workspace/composer.json
COPY testapps/SimplePHP/run.php /workspace/run.php
COPY testapps/SimplePHP/kafka_consumer.php /workspace/kafka_consumer.php
COPY testapps/SimplePHP/run_all.sh /workspace/run_all.sh

RUN docker-php-ext-install zip mysqli pdo pdo_mysql && docker-php-ext-enable pdo_mysql
RUN  pecl channel-update pecl.php.net && printf "\n" | pecl install rdkafka mongodb redis memcached  apcu 

COPY config.m4 /pinpoint-c-agent/config.m4 
COPY pinpoint_php.cpp /pinpoint-c-agent/pinpoint_php.cpp 
COPY php_pinpoint_php.h /pinpoint-c-agent/php_pinpoint_php.h
COPY common /pinpoint-c-agent/common
COPY tests /pinpoint-c-agent/tests

RUN cd /pinpoint-c-agent/ && phpize && ./configure && make && make install
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer

RUN COMPOSER_ALLOW_SUPERUSER=1 composer require pinpoint-apm/pinpoint-php-aop:dev-feat-async-rdkafka

CMD [ "sh" ,"/workspace/run_all.sh" ]
