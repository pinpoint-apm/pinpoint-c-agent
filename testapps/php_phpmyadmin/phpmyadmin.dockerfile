FROM phpmyadmin:apache

RUN apt update && apt-get install -y \
    libzip-dev \
    zip \
    git \
    && docker-php-ext-install zip

# COPY config.m4 /pinpoint-c-agent/config.m4 
# COPY pinpoint_php.cpp /pinpoint-c-agent/pinpoint_php.cpp 
# COPY php_pinpoint_php.h /pinpoint-c-agent/php_pinpoint_php.h
# COPY common /pinpoint-c-agent/common
# COPY testapps/php_wordpress/pinpoint_php.ini /pinpoint-c-agent/pinpoint_php.ini

# RUN cd /pinpoint-c-agent/ && phpize && ./configure && make install
# RUN INI_DIR=`php-config --ini-dir` && cp /pinpoint-c-agent/pinpoint_php.ini $INI_DIR

RUN curl -sL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/latest/download/install_pinpoint_php.sh | sh

COPY testapps/php_phpmyadmin/index.php /var/www/html/index.php
## install composer
COPY --from=composer:latest /usr/bin/composer /usr/local/bin/composer
RUN cd /var/www/html/ &&  composer require -w pinpoint-apm/pinpoint-php-aop