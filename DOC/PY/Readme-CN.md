## 支持计划
我们已经支持以及将要支持的计划详情: [支持计划](SupportPlan.md)

## 入门指南

### 要求

依赖| 版本
---|----
python |python 3.5+
python async|python 3.7.1+
GO | | 
gcc|gcc 4.7+
cmake| 3.0+
*inux| 
pinpoint| 2.0+(GRPC)

### 安装步骤

#### 搭建 pinpointPy 模块

1. 安装python虚拟环境，请参考
https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/

2. 安装 pinpointPy（建议在Python虚拟环境下执行）
```shell
$ pip install pinpointPy
```
#### 安装 Collector Agent
`Collector-Agent` 负责接收并格式化 `PHP/Python/C/CPP-Agent` 的请求数据然后转发给 `Pinpoint-Collector`。由于 `Collector-Agent` 使用[golang](https://golang.google.cn/) 语言编写， 请先安装golang。[Install GO](https://golang.google.cn/doc/install)

  1. 执行命令 `go build`
  2. 添加环境变量:
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
     1. `PP_COLLECTOR_AGENT_SPAN_IP`, `PP_COLLECTOR_AGENT_AGENT_IP`, `PP_COLLECTOR_AGENT_STAT_IP`: 设置为 `pinpoint-collector` 的IP.
     2. `PP_COLLECTOR_AGENT_SPAN_PORT`, `PP_COLLECTOR_AGENT_AGENT_PORT`, `PP_COLLECTOR_AGENT_STAT_PORT`: 设置为 `pinpoint-collector`(grpc) 的端口(默认9993，9992， 9991).
     3. `PP_LOG_DIR`: 设置 `Collector-Agent` 日志存放路径.
     4. `PP_Log_Level`: 设置日志的级别（DEBUG, INFO, WARN, ERROR）.
     5. `PP_ADDRESS`: 设置 `Collector-Agent` 的地址合端口，`PHP/Python-Agent` 将会通过这个地址连接 `pinpoint-collctor`。
  3. 运行 `Collector-Agent`，执行命令：`./CollectorAgent`
         
   `Collector-Agent` 数据的说明：
   [Json string map to Pinpoint item](../API/collector-agent/Readme.md)
   


### [如何使用]
[请点击 ☚](../../plugins/PY/Readme.md)


## 性能测试结果

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