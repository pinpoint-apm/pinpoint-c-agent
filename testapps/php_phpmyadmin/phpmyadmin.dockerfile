FROM phpmyadmin:apache

RUN apt update && apt-get install -y \
    libzip-dev \
    zip \
    git \
    && docker-php-ext-install zip

# COPY config.m4 /pinpoint-c-agent/config.m4 
# COPY src/PHP /pinpoint-c-agent/src/PHP
# COPY common /pinpoint-c-agent/common
# COPY testapps/php_wordpress/pinpoint_php.ini /pinpoint-c-agent/pinpoint_php.ini

# RUN cd /pinpoint-c-agent/ && phpize && ./configure && make install
# RUN INI_DIR=`php-config --ini-dir` && cp /pinpoint-c-agent/pinpoint_php.ini $INI_DIR

RUN curl -sL https://github.com/eeliu/pinpoint-c-agent/releases/download/v0.1.11/install_pinpoint_php.sh | sh

COPY testapps/php_phpmyadmin/index.php /var/www/html/index.php
## install composer
COPY --from=composer:latest /usr/bin/composer /usr/local/bin/composer
RUN cd /var/www/html/ &&  composer require -w pinpoint-apm/pinpoint-php-aop:v3.0.1