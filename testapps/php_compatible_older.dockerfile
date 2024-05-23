ARG PHP_VERSION=7.4
FROM  php:${PHP_VERSION}
WORKDIR /pinpoint-c-agent/
RUN apt update && apt install -y valgrind git
COPY config.m4 /pinpoint-c-agent/config.m4 
COPY src/PHP /pinpoint-c-agent/src/PHP
COPY common /pinpoint-c-agent/common
COPY tests /pinpoint-c-agent/tests
ENV NO_INTERACTION=1
RUN cd /pinpoint-c-agent/ && phpize && ./configure && make && make install
## install redis
RUN cd /tmp/ && git clone https://github.com/phpredis/phpredis.git && cd phpredis && phpize && ./configure &&  make install
RUN docker-php-ext-install mysqli pdo pdo_mysql && docker-php-ext-enable pdo_mysql

COPY testapps/shell.sh shell.sh
RUN chmod +x shell.sh
CMD ["sh", "shell.sh"]



ARG PHP_VERSION=7.1.33
FROM  gcc:10
WORKDIR /pinpoint-c-agent/
RUN apt update && apt install -y valgrind git
RUN  wget https://www.php.net/distributions/php-${PHP_VERSION}.tar.gz && tar xvf php-${PHP_VERSION}.tar.gz
RUN  cd php-${PHP_VERSION} && ./configure --build=x86_64-linux-gnu --with-config-file-path=/usr/local/etc/php --with-config-file-scan-dir=/usr/local/etc/php/conf.d --enable-option-checking=fatal --with-mhash --enable-ftp --enable-mbstring --enable-mysqlnd --with-pdo-sqlite=/usr --with-sqlite3=/usr --with-curl  --with-openssl --with-zlib --with-libdir=lib/x86_64-linux-gnu build_alias=x86_64-linux-gnu && make -j && make install

COPY config.m4 /pinpoint-c-agent/config.m4 
COPY src/PHP /pinpoint-c-agent/src/PHP
COPY common /pinpoint-c-agent/common
COPY tests /pinpoint-c-agent/tests
ENV NO_INTERACTION=1
RUN cd /pinpoint-c-agent/ && phpize && ./configure && make && make install
## install redis
RUN cd /tmp/ && git clone https://github.com/phpredis/phpredis.git && cd phpredis && phpize && ./configure &&  make install
RUN docker-php-ext-install mysqli pdo pdo_mysql && docker-php-ext-enable pdo_mysql

COPY testapps/shell.sh shell.sh
RUN chmod +x shell.sh
CMD ["sh", "shell.sh"]