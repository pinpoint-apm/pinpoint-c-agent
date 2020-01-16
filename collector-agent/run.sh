#!/usr/bin/env bash

PPATH=$(dirname "$0")
#export GRPC_TRACE=all
#export GRPC_VERBOSITY=INFO
#export GODEBUG=http2debug=1 
export GRPC_ENABLE_FORK_SUPPORT=1
python -m plop.collector -f flamegraph $PPATH/run.py
