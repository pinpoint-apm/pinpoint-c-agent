## 入门指南

### 要求

依赖| 版本
---|----
python |python 3.5+
python async|python 3.7.1+
gcc|gcc 4.7+
cmake| 3.0+
*inux| 
pinpoint| 2.0+(GRPC)

### 安装步骤

#### 搭建 pinpointPy 模块

1. 安装python虚拟环境，请参考
https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/

2. 回到根目录下，安装 pinpointPy（建议在Python虚拟环境下执行）
```shell
$ python setup.py install
```
#### 搭建 Collector-agent
1. 在 collector-agent 目录下，执行以下命令：
2. python3 -m venv env
3. source env/bin/activate
4. pip3 install -r requirements.txt
5. 根据以下内容更改 conf/collector.conf 中的配置文件

    ```ini
    [Collector]
    AgentID=dev
    ApplicationName=dev-app
 
    # pinpoint-collector host and specific port
    ### 要使用thrift协议，pinpoint的版本需要在 1.8.0-RC1 以上
    #CollectorSpanIp=collectorHost
    #CollectorSpanPort=9905
    #CollectorStatIp=collectorHost
    #CollectorStatPort=9906
    #CollectorTcpIp=collectorHost
    #CollectorTcpPort=9907
 
    ### 要使用grpc协议，pinpoint的版本需要在 2.0 以上
    AgentID=your_id
    ApplicationName=your_name
    collector.grpc.agent.ip=collectorHost
    collector.grpc.agent.port=9991
    collector.grpc.stat.ip=collectorHost
    collector.grpc.stat.port=9992
    collector.grpc.span.ip=collectorHost
    collector.grpc.span.port=9993
 
    [Common]
    # your web server (nginx&apache) port
    Web_Port=8001   
    # debug in dev
    Log_Level=ERROR 
    # 确保 LOG_DIR 是存在的
    # 可以使用“tail -f”命令在此路径下查看collector-agent的实时日志
    LOG_DIR=/your log dir/ 
    [Agent]
    # the same as below "pinpoint_php.CollectorHost"
    # sock address
    Address=/tmp/collector-agent.sock
    # or TCP address
    #Address=ip@port
    ```
6. export COLLECTOR_CONFIG=/full path of collector.conf/
7. collector-agent的启动命令如下所示：
    ```
    $ ./init_python_env.sh
    $ python run.py 
    ```

### [如何使用]
[请点击 ☚](../../Example/PY/Readme-CN.md)


## 变化
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

> TPR:Time per request（每个请求的响应时间）        
> RPS:Requests per second（每秒的请求数量）
