#!/bin/bash
export PP_COLLECTOR_AGENT_SPAN_IP=dev-pinpoint
export PP_COLLECTOR_AGENT_SPAN_PORT=9993
export PP_COLLECTOR_AGENT_AGENT_IP=dev-pinpoint
export PP_COLLECTOR_AGENT_AGENT_PORT=9991
export PP_COLLECTOR_AGENT_STAT_IP=dev-pinpoint
export PP_COLLECTOR_AGENT_STAT_PORT=9992
export PP_COLLECTOR_AGENT_ISDOCKER=false
# export PP_LOG_DIR=/tmp/
export PP_Log_Level=DEBUG
export PP_ADDRESS=0.0.0.0@9999
export GO_PATH=/home/pinpoint/go/bin
export PATH=$PATH:$GO_PATH
make && ./collector-agent