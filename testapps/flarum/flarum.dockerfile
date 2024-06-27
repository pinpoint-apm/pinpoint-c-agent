FROM mondedie/flarum:latest

RUN apk update && apk add php8-dev build-base


RUN curl -sL https://github.com/eeliu/pinpoint-c-agent/releases/download/v0.1.11/install_pinpoint_php.sh | sh
COPY testapps/flarum/site.php /flarum/app/site.php
RUN cd /flarum/app/ && composer require -w pinpoint-apm/pinpoint-php-aop:dev-feat-use-class-map-script