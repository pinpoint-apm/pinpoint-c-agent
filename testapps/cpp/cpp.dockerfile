FROM gcc:10

RUN apt update && apt install -y cmake 
COPY testapps/cpp/ /apps
WORKDIR /apps
RUN mkdir -p build && cd build && cmake .. && make

EXPOSE 8080

ENTRYPOINT [ "build/server" ]