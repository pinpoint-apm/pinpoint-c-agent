FROM ubuntu:20.04
ENV TZ=Asia/Shanghai
ENV LC_CTYPE=en_US.UTF-8
WORKDIR /workspace
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN DEBIAN_FRONTEND="noninteractive" apt update && apt-get install -y build-essential git php7.4 php7.4-dev  php7.4-curl  php7.4-mysql
COPY testapps/SimplePHP/php.ini /etc/php/7.4/cli/php.ini
COPY testapps/SimplePHP /workspace
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer
RUN git clone --recurse-submodules --depth 1 --branch 1.18.1 https://github.com/mongodb/mongo-php-driver.git && cd mongo-php-driver && phpize && ./configure && make -j && make install 
RUN COMPOSER_ALLOW_SUPERUSER=1 composer update

COPY config.m4 /pinpoint-c-agent/config.m4 
COPY src/PHP /pinpoint-c-agent/src/PHP
COPY common /pinpoint-c-agent/common
COPY tests /pinpoint-c-agent/tests

RUN cd /pinpoint-c-agent/ && phpize && ./configure && make && make install

CMD [ "php" ,"/workspace/run.php" ]
