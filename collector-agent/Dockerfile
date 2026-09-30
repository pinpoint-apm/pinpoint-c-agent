FROM golang:1.27.1-alpine3.23 AS builder
WORKDIR /apps
USER root
RUN apk add --no-cache make protobuf-dev git
RUN go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.33.0 && go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.3.0 

# Build context is the repo root. Copy the collector-agent source (which
# contains the makefile) plus the repo-root versions.json (the single source of
# truth for the version). The makefile reads ../versions.json, so versions.json
# must land at /versions.json while the source lands at /apps.
COPY collector-agent/ /apps
COPY versions.json /versions.json
RUN make

FROM alpine:3.17
COPY --from=builder /apps/collector-agent /usr/local/bin/CollectorAgent
LABEL org.opencontainers.image.source=https://github.com/pinpoint-apm/pinpoint-c-agent.git
LABEL org.opencontainers.image.description="collector agent for pinpoint-collector"
LABEL org.opencontainers.image.licenses="Apache-2.0"
ENTRYPOINT [ "CollectorAgent" ]