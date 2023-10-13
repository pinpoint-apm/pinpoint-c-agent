FROM golang:1.18-alpine3.17 as builder
WORKDIR /apps
COPY  collector-agent /apps/collector-agent
RUN  cd /apps/collector-agent && go mod tidy && go build  -o  CollectorAgent

FROM alpine:3.17
COPY --from=builder /apps/collector-agent/CollectorAgent /usr/local/bin
ENTRYPOINT [ "CollectorAgent" ]