FROM php:8.0.30-cli


RUN apt update && apt-get install -y \
    libzip-dev \
    zip \
    git \
    && docker-php-ext-install zip pdo pdo_mysql

RUN curl -sL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.6.0/install_pinpoint_php.sh | sh

COPY testapps/thinkphp-php/tp /app

COPY testapps/thinkphp-php/index.php /app/public/index.php

RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer

RUN cd /app && composer update && composer require -w pinpoint-apm/pinpoint-php-aop
WORKDIR /app
EXPOSE 8000
CMD [ "php" ,"think", "run" ]