FROM golang:1.18-alpine3.17 as builder
WORKDIR /apps
USER root
RUN apk add --no-cache make protobuf-dev
RUN go install google.golang.org/protobuf/cmd/protoc-gen-go@latest && go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
COPY ./ /apps
RUN make

FROM alpine:3.17
COPY --from=builder /apps/collector-agent /usr/local/bin/CollectorAgent
ENTRYPOINT [ "CollectorAgent" ]