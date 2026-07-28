# syntax=docker/dockerfile:1
FROM ubuntu:20.04
SHELL ["/bin/bash", "-c"]
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
RUN apt update && apt install -y apt-utils gcc g++ gdb  pkg-config  libxml2-dev libsqlite3-dev libcurl4-openssl-dev zlib1g-dev make cmake wget autoconf git clangd iputils-ping python3 python3.8-venv python3-pip clang-format protobuf-compiler vim locales curl libssl-dev libmysqlclient-dev
# https://github.com/git-lfs/git-lfs/blob/main/INSTALLING.md
RUN curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash 
RUN apt-get install git-lfs
USER pinpoint
SHELL ["/bin/bash", "-c"]
