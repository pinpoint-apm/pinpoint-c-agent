
TAG ?=v0.5.2

.PHONY: default
default: server ;

protoc:
	protoc --go_out=pinpoint-grpc-idl/proto/ --go-grpc_out=pinpoint-grpc-idl/proto  --proto_path=pinpoint-grpc-idl/proto/ --go-grpc_opt=paths=source_relative v1/Annotation.proto v1/Cmd.proto v1/CustomMetric.proto v1/Service.proto v1/Span.proto v1/Stat.proto v1/ThreadDump.proto

server: protoc
	go mod tidy
	go build -ldflags "-X 'github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/server.Version=${TAG}'" .

clean:
	rm -rf pinpoint-grpc-idl/proto/v1/*.go
	rm -rf collector-agent

docker:
	docker build  -f Dockerfile -t collector-agent:${TAG}  . 