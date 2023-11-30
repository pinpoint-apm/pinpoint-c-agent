## Install Collector Agent
`Collector-Agent` formats the span from PHP/Python/C/CPP-Agent and send to `Pinpoint-Collector`.

### 1. build from source
1. Goto collector-agent(`pinpoint-c-agent/collector-agent`)
2. Execute command `make`
3. Add environment variables:
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
    3. `PP_LOG_DIR`: Set the path of Collector-Agent's log file.
    4. `PP_Log_Level`: Set the log level.
    5. `PP_ADDRESS`: Set the address of `Collector-Agent`, then `PHP/Python-Agent` will connect Collector-Agent through this address.
4. Run `Collector-Agent` by executing command `./CollectorAgent`
   
### 2. Use docker images

```sh
docker run -itd -p 9999:9999  --env-file ./env.list ghcr.io/pinpoint-apm/pinpoint-c-agent/collector-agent:0.4.15
```
### 3. K8s side car

server.yaml sample

``` yml
- image: ghcr.io/pinpoint-apm/pinpoint-c-agent/collector-agent:0.4.14
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
