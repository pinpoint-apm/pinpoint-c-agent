## Collector Agent 安装步骤

#### 步骤
在 collector-agent 目录下，执行以下命令：
   1. python3 -m venv env
   2. source env/bin/activate
   3. pip3 install -r requirements.txt
   4. 根据以下内容更改 conf/collector.conf 中的配置文件

        ```ini
        [Collector]
        # collectorhost and specific port
        ### 要使用thrift协议，pinpoint的版本需要在 1.8.0-RC1 以上
        #CollectorSpanIp=collectorHost
        #CollectorSpanPort=9905
        #CollectorStatIp=collectorHost
        #CollectorStatPort=9906
        #CollectorTcpIp=collectorHost
        #CollectorTcpPort=9907
        ### 要使用grpc协议，pinpoint的版本需要在 2.0 以上
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
        
   5. export COLLECTOR_CONFIG=/full path of collector.conf/
   6. Collector Agent的启动命令如下所示：
        ```
        $ ./init_python_env.sh
        $ python run.py 
        ```
## Collector Agent Span Specification
[Json string map to Pinpoint item](../API/collector-agent/Readme.md)