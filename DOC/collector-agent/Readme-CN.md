[English](Readme.md) | [中文](Readme-CN.md) | [한국어](Readme-KR.md)

## Collector-agent 作用

pinpoint-collector的桥接模块

![collector-agent](/images/pinpoint_v0.5.x.png)

## Collector Agent 介绍
`Collector-Agent` 格式从PHP/Python/C/CPP-Agent 发送过来的Span，并转发到`Pinpoint-Collector`。

- 配置环境变量:
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
    export PP_ADDRESS=0.0.0.0@10000
    ```
    1. `PP_COLLECTOR_AGENT_SPAN_IP`, `PP_COLLECTOR_AGENT_AGENT_IP`, `PP_COLLECTOR_AGENT_STAT_IP`: Set the IP of pinpoint-collector.
    2. `PP_COLLECTOR_AGENT_SPAN_PORT`, `PP_COLLECTOR_AGENT_AGENT_PORT`, `PP_COLLECTOR_AGENT_STAT_PORT`: Set the port of pinpoint-collector(grpc).
    3. `PP_LOG_DIR`: Set the path of collector-agent's log file.
    4. `PP_Log_Level`: Set the log level.
    5. `PP_ADDRESS`: Set the address of `collector-agent`, then `PHP/Python-Agent` will connect collector-agent through this address.

### 1. 从github release 页面下载二进制程序

  https://github.com/pinpoint-apm/pinpoint-c-agent/releases/tag/v0.6.6

### 2. 直接使用docker image

> 发布页面:  https://github.com/pinpoint-apm/pinpoint-c-agent/pkgs/container/pinpoint-c-agent%2Fcollector-agent

#### 例子:

```sh
docker run -itd -p 9999:9999  --env-file ./env.list ghcr.io/pinpoint-apm/pinpoint-c-agent/collector-agent:v0.6.6
```

### 3. K8S 中以sidecar 模式运行

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

### 4. 手动编译 [require `go.1.25`]

- 执行: `GOBIN=/usr/local/bin/ go install github.com/pinpoint-apm/pinpoint-c-agent/collector-agent@dev`
  - ❗❗❗Note: collector-agent: located into `/usr/local/bin/`

`./collector-agent`
   