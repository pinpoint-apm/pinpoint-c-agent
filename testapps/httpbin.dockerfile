FROM kennethreitz/httpbin:latest
USER root
RUN apt update && apt install -y curl