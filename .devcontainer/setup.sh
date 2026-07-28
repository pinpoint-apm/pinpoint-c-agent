#!/bin/bash
set -e

# enable php-7.4
export PHP_VESION=php-7.4.33
export PHP_HOME=/home/pinpoint/local

if [ ! -d "$PHP_HOME/$PHP_VESION" ]; then
    echo "Installing $PHP_VESION..."
    mkdir -p $PHP_HOME
    wget https://github.com/eeliu/pinpoint-c-agent/files/13371519/$PHP_VESION.tar.gz  -O /home/pinpoint/$PHP_VESION.tar.gz
    cd /home/pinpoint/ && tar xvf $PHP_VESION.tar.gz && cd $PHP_VESION
    ./configure --prefix=$PHP_HOME/$PHP_VESION --with-curl=shared --with-mysqli=shared --with-openssl=shared --with-pdo-sqlite=shared --enable-pdo=shared --with-zlib=shared --enable-fpm --enable-opcache --enable-debug
    make -j$(nproc)
    make install
    rm /home/pinpoint/$PHP_VESION.tar.gz
    # Create symlink to match the PATH
    ln -snf $PHP_HOME/$PHP_VESION $PHP_HOME/php
fi

# install golang 1.25
if [ ! -d "/usr/local/go" ]; then
    echo "Installing Go 1.25..."
    wget https://go.dev/dl/go1.25.0.linux-amd64.tar.gz -O /tmp/go1.25.0.linux-amd64.tar.gz
    sudo rm -rf /usr/local/go && sudo tar -C /usr/local -xzf /tmp/go1.25.0.linux-amd64.tar.gz
    rm /tmp/go1.25.0.linux-amd64.tar.gz
fi

export PATH=$PATH:/usr/local/go/bin
export PATH=$PATH:/home/pinpoint/go/bin
export GO111MODULE="on"

echo "Installing protoc-gen-go plugins..."
go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.31.0
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.3.0

# Add environment variables to .bashrc if not present
if ! grep -q "PHP_HOME" $HOME/.bashrc; then
    echo "" >> $HOME/.bashrc
    echo "# Pinpoint Dev Environment" >> $HOME/.bashrc
    echo "export PHP_VESION=$PHP_VESION" >> $HOME/.bashrc
    echo "export PHP_HOME=$PHP_HOME" >> $HOME/.bashrc
    echo 'export PATH="$PATH:$PHP_HOME/php/bin/:$PHP_HOME/php/sbin/"' >> $HOME/.bashrc
    echo 'export PATH=$PATH:/usr/local/go/bin' >> $HOME/.bashrc
    echo 'export PATH=$PATH:/home/pinpoint/go/bin' >> $HOME/.bashrc
    echo 'export GO111MODULE="on"' >> $HOME/.bashrc
fi

echo "Setup completed successfully."
