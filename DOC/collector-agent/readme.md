## What's collector-agent

A bridger to pinpoint-collector. 

![collector-agent](/images/pinpoint_v0.5.x.png)

## Collector Agent Guide
`Collector-Agent` formats the span from PHP/Python/C/CPP-Agent and send to `Pinpoint-Collector`.

- Add environment variables:
    ```
    export PP_COLLECTOR_AGENT_SPAN_IP=dev-pinpoint
    export PP_COLLECTOR_AGENT_SPAN_PORT=9993
    export PP_COLLECTOR_AGENT_AGENT_IP=dev-pinpoint
    export PP_COLLECTOR_AGENT_AGENT_PORT=9991
    export PP_COLLECTOR_AGENT_STAT_IP=dev-pinpoint
    export PP_COLLECTOR_AGENT_STAT_PORT=9992
    export PP_COLLECTOR_AGENT_ISDOCKER=false
    export PP_LOG_DIR=/tmp/
    export PP_Log_Level=INFO
    export PP_ADDRESS=0.0.0.0@9999
    ```
    1. `PP_COLLECTOR_AGENT_SPAN_IP`, `PP_COLLECTOR_AGENT_AGENT_IP`, `PP_COLLECTOR_AGENT_STAT_IP`: Set the IP of pinpoint-collector.
    2. `PP_COLLECTOR_AGENT_SPAN_PORT`, `PP_COLLECTOR_AGENT_AGENT_PORT`, `PP_COLLECTOR_AGENT_STAT_PORT`: Set the port of pinpoint-collector(grpc).
    3. `PP_LOG_DIR`: Set the path of collector-agent's log file.
    4. `PP_Log_Level`: Set the log level.
    5. `PP_ADDRESS`: Set the address of `collector-agent`, then `PHP/Python-Agent` will connect collector-agent through this address.

### 1. Download from github.release

  https://github.com/pinpoint-apm/pinpoint-c-agent/releases/latest

### 2. Use docker images

> Release page:  https://github.com/pinpoint-apm/pinpoint-c-agent/pkgs/container/pinpoint-c-agent%2Fcollector-agent

#### Example:

```sh
docker run -itd -p 9999:9999  --env-file ./env.list ghcr.io/pinpoint-apm/pinpoint-c-agent/collector-agent:latest
```

### 3. K8s side car

server.yaml sample

``` yml
- image: ghcr.io/pinpoint-apm/pinpoint-c-agent/collector-agent:v0.6.4
        name: collector-agent
        args: ["-RecvBufSize=1048576"]
        securityContext:
          runAsUser: 0
        env:
          - name: "PP_COLLECTOR_AGENT_SPAN_IP"
            value: "pinpoint-collector.hostname"
          - name: "PP_COLLECTOR_AGENT_SPAN_PORT"
            value: "9993"
          - name: "PP_COLLECTOR_AGENT_AGENT_IP"
            value: "pinpoint-collector.hostname"
          - name: "PP_COLLECTOR_AGENT_AGENT_PORT"
            value: "9991"
          - name: "PP_COLLECTOR_AGENT_STAT_IP"
            value: "pinpoint-collector.hostname"
          - name: "PP_COLLECTOR_AGENT_STAT_PORT"
            value: "9992"
          - name: "PP_Log_Level"
            value: "INFO"
          - name: "PP_ADDRESS"
            value: "localhost@9999"
          - name: "PP_COLLECTOR_AGENT_ISDOCKER"
            value: "true"
```

### 4. Compiling from source [require `go.1.18`]

- Install: `GOBIN=/usr/local/bin/ go install github.com/pinpoint-apm/pinpoint-c-agent/collector-agent@dev`
  - ❗❗❗Note: collector-agent: located into `/usr/local/bin/`
  
`./collector-agent`
   