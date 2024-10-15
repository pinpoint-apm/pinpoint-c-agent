FROM gcc:10

RUN apt update && apt install -y cmake 
COPY testapps/libevent_http_server/ /apps
WORKDIR /apps
RUN mkdir -p build && cd build && cmake .. && make

EXPOSE 8080

ENTRYPOINT ["build/server","-p","8080", "-H","0.0.0.0","."]