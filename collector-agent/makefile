
TAG ?=0.7.1

.PHONY: default
default: server ;

# Regenerates the Go protobuf code under pinpoint-grpc-idl-go/proto/v1 from the
# .proto files in the pinpoint-grpc-idl submodule. The generated *.pb.go files
# are NOT committed to the repo (see .gitignore) and are produced at build time.
#
# Required tool versions (must match the project's go 1.19 / grpc v1.56.3):
#   protoc-gen-go      v1.33.0   (newer versions emit unsafe.StringData, needs go1.20+)
#   protoc-gen-go-grpc v1.3.0    (newer versions emit generic streaming API, needs grpc v1.64+)
# Install with:
#   go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.33.0
#   go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@v1.3.0
protoc:
	protoc --go_out=pinpoint-grpc-idl-go/proto/ --go-grpc_out=pinpoint-grpc-idl-go/proto  --proto_path=pinpoint-grpc-idl/proto/ --go-grpc_opt=paths=source_relative v1/Annotation.proto v1/Cmd.proto v1/CustomMetric.proto v1/Service.proto v1/Span.proto v1/Stat.proto v1/ThreadDump.proto

server: protoc
	go mod tidy
	# Version is hardcoded in server/Server.go; do not inject via -ldflags.
	go build .

clean:
	rm -rf pinpoint-grpc-idl/proto/v1/*.go
	rm -rf collector-agent

test: server
	go test ./... -v	

docker:
	docker build  -f Dockerfile -t collector-agent:${TAG}  . 