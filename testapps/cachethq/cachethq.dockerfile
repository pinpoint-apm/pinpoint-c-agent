FROM cachethq/docker:latest
USER root
RUN apk update && apk add --virtual build-dependencies build-base gcc wget git php7-dev
ENV DB_DRIVER=pgsql
COPY config.m4 /pinpoint-c-agent/config.m4 
COPY src/PHP /pinpoint-c-agent/src/PHP
COPY common /pinpoint-c-agent/common
COPY testapps/php_wordpress/pinpoint_php.ini /pinpoint-c-agent/pinpoint_php.ini


RUN cd /pinpoint-c-agent/ && phpize && ./configure && make install
RUN  cp /pinpoint-c-agent/pinpoint_php.ini /etc/php7/conf.d/

COPY testapps/cachethq/index.php /var/www/html/public/index.php
# CMD [ "bash" ]
# COPY --from=composer:latest /usr/bin/composer /usr/local/bin/composer
RUN /bin/composer.phar self-update
USER 1001
RUN cd /var/www/html/ && php -d memory_limit=-1 /bin/composer.phar require  pinpoint-apm/pinpoint-php-aop
# USER root