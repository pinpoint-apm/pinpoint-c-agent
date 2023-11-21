FROM yiisoftware/yii2-php:7.4-fpm-nginx


RUN composer create-project --prefer-dist yiisoft/yii2-app-basic /app
RUN composer require pinpoint-apm/pinpoint-php-aop:v2.1.0
RUN chown -R www-data:www-data /app
# Copy index.php
COPY testapps/PHP/index.php /app/web/index.php
COPY testapps/PHP/php.ini   /usr/local/etc/php/conf.d/pinpoint-php-ext.ini
# build ext
COPY config.m4 /pinpoint-c-agent/config.m4 
COPY src/PHP /pinpoint-c-agent/src/PHP
COPY common /pinpoint-c-agent/common
COPY tests /pinpoint-c-agent/tests

RUN cd /pinpoint-c-agent/ && phpize && ./configure && make && make install
EXPOSE 80