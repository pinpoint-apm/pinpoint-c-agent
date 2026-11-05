FROM golang:1.27.3-alpine3.23 AS builder
WORKDIR /apps
USER root
RUN apk add --no-cache make protobuf-dev git
RUN go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.33.0 && go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.3.0 

# Build context is the repo root; the makefile contains the synchronized version.
COPY collector-agent/ /apps
RUN make

FROM alpine:3.23
COPY --from=builder /apps/collector-agent /usr/local/bin/CollectorAgent
RUN mkdir -p /usr/share/doc/collector-agent
COPY collector-agent/CHANGES.md /usr/share/doc/collector-agent/CHANGES.md
LABEL org.opencontainers.image.source=https://github.com/pinpoint-apm/pinpoint-c-agent.git
LABEL org.opencontainers.image.description="collector agent for pinpoint-collector"
LABEL org.opencontainers.image.licenses="Apache-2.0"
ENTRYPOINT [ "CollectorAgent" ]
