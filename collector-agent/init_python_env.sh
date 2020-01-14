#!/bin/bash
pip install grpcio-tools
python -m grpc_tools.protoc -I./Proto/grpc --python_out=./Proto/grpc --grpc_python_out=./Proto/grpc ./Proto/grpc/*.proto
# /home/test/tools/Python-3.8.0/Tools/gdb/