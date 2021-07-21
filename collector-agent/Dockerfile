FROM golang:1.16.5-alpine3.13 as builder
WORKDIR /apps
COPY  . /apps/collector-agent
RUN  cd /apps/collector-agent && go build

FROM centos:7
COPY --from=builder /apps/collector-agent/CollectorAgent /usr/local/bin
ENTRYPOINT [ "CollectorAgent" ]