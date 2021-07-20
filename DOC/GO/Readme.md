## Getting Started

### Requirement

Dependency| Version| More
---|----|---
GO | | 
gcc|gcc 4.7+| c++11
cmake| 3.13+ | ✔
*inux|  | 
pinpoint| 2.0+(GRPC)|

### Installation

#### Install Pinpoint Lib

> Steps

1. Execute commands below:
    ```shell
     $ git clone https://github.com/pinpoint-apm/pinpoint-c-agent.git
     $ cd pinpoint-c-agent/common
     $ make && make install
    ```
    If the installation completes successfully, you will see the infomation like following:
    ```
    ...
    Install the project...
    -- Install configuration: "Release"
    -- Up-to-date: /usr/local/include/pinpoint_common
    -- Up-to-date: /usr/local/include/pinpoint_common/common.h
    -- Installing: /usr/local/lib64/libpinpoint_common_shared.so
    -- Installing: /usr/local/lib64/libpinpoint_common.a
    -- Installing: /usr/local/lib64/pkgconfig/pinpoint_common.pc
    ```
    ps: The real install path depends on you enviroment, for the information above, the install path is `/usr/local/lib64`
2. Export the pinpoint lib installing path to `LD_LIBRARY_PATH` `PKG_CONFIG_PATH` `LD_RUN_PATH`
```shell
export LD_LIBRARY_PATH=/path to pinpoint lib/:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/path to pinpoint lib/:/path to pinpoint lib/pkgconfig:$PKG_CONFIG_PATH
export LD_RUN_PATH=/path to pinpoint lib/:$LD_RUN_PATH
```

#### Install Collector Agent
`Collector-Agent`, who formats the span from PHP/Python/C/CPP-Agent and send to `Pinpoint-Collector`, is an agent written by [golang](https://golang.google.cn/).Please install golang before the following steps.[Install GO](https://golang.google.cn/doc/install)

1. Goto collector-agent(`pinpoint-c-agent/collector-agent`)
2. Execute command `go build`
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
   
  Collector Agent Span Specification
  [Json string map to Pinpoint item](../API/collector-agent/Readme.md)

Now the installtion of go-agent has been completed!

### [How to Use]
[I use PYTHON ☚](https://oss.navercorp.com/pinpoint/go-aop-agent)
