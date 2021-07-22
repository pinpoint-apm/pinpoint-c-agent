#!/bin/bash
docker run -itd -p 9999:9999  --env-file ./env.list ghcr.io/pinpoint-apm/pinpoint-c-agent/collector-agent:latest