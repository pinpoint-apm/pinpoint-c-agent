FROM shyim/shopware:6.4.20-php8.2

RUN apk update && apk add --virtual build-dependencies build-base git autoconf

RUN curl -sL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.6.0/install_pinpoint_php.sh | sh

## install composer
RUN composer self-update 2.4.4
USER  www-data 
COPY testapps/shopware/index.php /var/www/html/public/index.php
RUN cd /var/www/html/ &&  composer require -w pinpoint-apm/pinpoint-php-aop
USER root