## Support Plan
What we have supported and what we are going to support: [support plan](SupportPlan.md)

## Getting Started

### Requirement

Dependency| Version| More
---|----|---
python |2.7,3+ | (async must 3.7.1+)
GO | | 
gcc|gcc 4.7+| c++11
cmake| 3.1+| ✔
*inux|  | `windows` is on the way
pinpoint| 2.0+(GRPC)|

### Installation

#### pinpointPy 

```shell
$ pip install pinpointPy
```
### Download pinpoint-python-plugins

[pinpoint-python-plugins.tar.gz](https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.4.0/pinpoint-py-v0.4.0.zip)

#### Install Collector Agent
`Collector-Agent`, who formats the datas from PHP/Python/C/CPP-Agent and send to `Pinpoint-Collector`, is an agent written by [golang](https://golang.google.cn/).Please install golang before the following steps.[Install GO](https://golang.google.cn/doc/install)

1. Goto collector-agent(`pinpoint-c-agent/collector-agent`)
2. Execute command `go build`
3. Add requirement variables:
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
    1. `PP_COLLECTOR_AGENT_SPAN_IP`, `PP_COLLECTOR_AGENT_AGENT_IP`, `PP_COLLECTOR_AGENT_STAT_IP`: Set the IP of pinpoint-collctor.
    2. `PP_COLLECTOR_AGENT_SPAN_PORT`, `PP_COLLECTOR_AGENT_AGENT_PORT`, `PP_COLLECTOR_AGENT_STAT_PORT`: Set the port of pinpoint-collctor(grpc).
    3. `PP_LOG_DIR`: Set the path of Collector-Agent's log file.
    4. `PP_Log_Level`: Set the log level.
    5. `PP_ADDRESS`: Set the address of Collector-Agent, then PHP/Python-Agent will connect Collector-Agent through this address.
4. Run Collector-Agent by execute command `./CollectorAgent`
   
  Collector Agent Span Specification
  [Json string map to Pinpoint item](../API/collector-agent/Readme.md)

### [How to Use]
[Click me ☚](../../plugins/PY/Readme.md)


## Performance Test Result

### Case: flask/test_mysql

-|TPR(ms)|RPS(#/sec)
----|-----|----
pinpoint-py|4.487|445.73|
-|4.498 |444.69
-|4.526 |441.88
pure|4.440|450.44
-|4.479|446.51
-|4.425|451.96
Result|+0.05ms|-1%

> TPR: time per request         
> RPS: requests per second