FROM ubuntu:20.04
USER root 
## install developmental env
ENV TZ=Asia/Shanghai
ENV LC_CTYPE=en_US.UTF-8
# ENV LC_ALL=en_US.UTF-8
# referr from https://code.visualstudio.com/remote/advancedcontainers/add-nonroot-user
ARG USERNAME=pinpoint
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    #
    # [Optional] Add sudo support. Omit if you don't need to install software after connecting.
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# ********************************************************
# * Anything else you want to do like clean up goes here *
# ********************************************************

# [Optional] Set the default user. Omit if you want to keep the default as root.
# USER $USERNAME


RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt update && apt install -y apt-utils gcc g++ gdb  pkg-config  libxml2-dev libsqlite3-dev libcurl4-openssl-dev zlib1g-dev make cmake wget autoconf git clangd iputils-ping python3 python3.8-venv python3-pip clang-format protobuf-compiler vim locales curl libssl-dev
# https://github.com/git-lfs/git-lfs/blob/main/INSTALLING.md
RUN curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash 
RUN apt-get install git-lfs
USER pinpoint 
# enable php-7.4
ENV PHP_VESION=php-7.4.33
ENV PHP_HOME=/home/pinpoint/local/
COPY ${PHP_VESION}.tar.gz /home/pinpoint/${PHP_VESION}.tar.gz
RUN cd /home/pinpoint/ && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=${PHP_HOME}/${PHP_VESION} --with-curl=shared --with-mysqli=shared --with-openssl=shared --with-pdo-sqlite=shared --enable-pdo=shared --with-zlib=shared --enable-fpm --enable-opcache --enable-debug && make -j && make install

ENV PATH="$PATH:${PHP_HOME}/php/bin/:${PHP_HOME}/php/sbin/"
COPY php.ini ${PHP_HOME}/php/lib

### Build php module 
### curl 
# RUN cd ~/${PHP_VESION}/ext/curl && phpize && ./configure && make -j && make install
### mysqli
# RUN cd ~/php-7.4.33/ext/mysqli && phpize && ./configure && make -j && make install
### pdo
# RUN cd ~/${PHP_VESION}/ext/pdo && phpize && ./configure && make -j && make install

# ENV PHP_VESION=php-5.6.30
# RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install


# ENV PHP_VESION=php-5.5.38
# RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install

# ENV PHP_VESION=php-8.0.29
# RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install

# ENV PHP_VESION=php-8.1.21
# RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install

# ENV PHP_VESION=php-8.2.10
# RUN cd ~ && wget https://www.php.net/distributions/${PHP_VESION}.tar.gz && tar xvf ${PHP_VESION}.tar.gz && cd ${PHP_VESION} && ./configure --prefix=/opt/${PHP_VESION} --enable-fpm   --enable-opcache   --enable-debug && make -j && make install


# install golang 1.18
USER root
COPY go1.18.10.linux-amd64.tar.gz /home/pinpoint/go1.18.10.linux-amd64.tar.gz
RUN rm -rf /usr/local/go && cd /home/pinpoint/ && tar -C /usr/local -xzf go1.18.10.linux-amd64.tar.gz
ENV PATH=$PATH:/usr/local/go/bin
ENV 
ENV PATH=$PATH:/home/pinpoint/go/bin
RUN go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.28 && go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.2