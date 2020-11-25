## Collector Agent Installation

#### Steps
Goto collector-agent
   1. python3 -m venv env
   2. source env/bin/activate
   3. pip3 install -r requirements.txt
   4. Change configuration file in conf/collector.conf

        ```ini
        [Collector]
        # collectorhost and specific port
        ### For thrift  pinpoint 1.8.0-RC1+
        #CollectorSpanIp=collectorHost
        #CollectorSpanPort=9905
        #CollectorStatIp=collectorHost
        #CollectorStatPort=9906
        #CollectorTcpIp=collectorHost
        #CollectorTcpPort=9907
        ### For GRPC  pinpoint 2.0+
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
        # make sure LOG_DIR is exist
        # The real-time log of collector-agent can be checked under this path with the command " tail -f ".
        LOG_DIR=/your log dir/ 
        [Agent]
        # the same as below "pinpoint_php.CollectorHost"
        # sock address
        Address=/tmp/collector-agent.sock
        # or TCP address
        #Address=ip@port
        ```
        
   5. export COLLECTOR_CONFIG=/full path of collector.conf/
   6. run  Collector Agent
        ```
        $ ./init_python_env.sh
        $ python run.py 
        ```
## Collector Agent Span Specification
[Json string map to Pinpoint item](../API/collector-agent/Readme.md)