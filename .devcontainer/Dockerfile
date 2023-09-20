FROM ubuntu:20.04
## install developmental env
ENV TZ=Asia/Shanghai
ENV LC_CTYPE=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt update && apt install -y gcc g++ gdb  pkg-config  libxml2-dev libsqlite3-dev libcurl4-openssl-dev zlib1g-dev make cmake wget autoconf git clangd python3-pip
## enable php-7.4
ENV PHP_VESION=php-7.4.33
RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install
ENV PATH="$PATH:/opt/php/bin/:/opt/php/sbin/"
COPY php.ini /opt/

### Build php module 
### curl 
# RUN cd ~/${PHP_VESION}/ext/curl && phpize && ./configure && make -j && make install
### mysqli
# RUN cd ~/php-7.4.33/ext/mysqli && phpize && ./configure && make -j && make install
### pdo
# RUN cd ~/${PHP_VESION}/ext/pdo && phpize && ./configure && make -j && make install

ENV PHP_VESION=php-5.6.30
RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install


ENV PHP_VESION=php-5.5.38
RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install

ENV PHP_VESION=php-8.0.29
RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install

ENV PHP_VESION=php-8.1.21
RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install

ENV PHP_VESION=php-8.2.10
RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install