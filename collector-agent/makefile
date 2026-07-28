
.PHONY: default
default: server ;

protoc:
	mkdir -p pinpoint-grpc-idl-go/proto
	protoc --go_out=pinpoint-grpc-idl-go/proto/ --go-grpc_out=pinpoint-grpc-idl-go/proto  --proto_path=pinpoint-grpc-idl/proto/ --go-grpc_opt=paths=source_relative --go_opt=Mv1/Log.proto=./v1 --go-grpc_opt=Mv1/Log.proto=./v1 v1/Annotation.proto v1/Cmd.proto v1/CustomMetric.proto v1/Log.proto v1/Service.proto v1/Span.proto v1/Stat.proto v1/ThreadDump.proto

server: protoc
	go mod tidy
	go build .

clean:
	rm -rf pinpoint-grpc-idl-go/proto/
	rm -f collector-agent

test: server
	go test ./... -v	

docker:
	docker build  -f Dockerfile -t collector-agent:dev  . 