## Getting Started

### Requirement

Dependency| Version
---|----
python |python 3.5+
python async|python 3.7.1+
gcc|gcc 4.7+
cmake| 3.0+
*inux| 
pinpoint| 1.8.0-RC1(thrift) <br> 2.0+(GRPC)

### Installation

#### Build pinpointPy module

1. Install python virtual environment,refer to 
https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/

2. Come back to the root directory, install pinpointPy (Python virtual environment is recommended).
```shell
$ python setup.py install
```
#### Build Collector-agent
1. Goto collector-agent
2. python3 -m venv env
3. source env/bin/activate
4. pip3 install -r requirements.txt
5. Change configuration file in conf/collector.conf

    ```ini
    [Collector]
    AgentID=dev
    ApplicationName=dev-app
 
    # pinpoint-collector host and specific port
    ### For thrift pinpoint 1.8.0-RC1+
    #CollectorSpanIp=collectorHost
    #CollectorSpanPort=9905
    #CollectorStatIp=collectorHost
    #CollectorStatPort=9906
    #CollectorTcpIp=collectorHost
    #CollectorTcpPort=9907
 
    ### For GRPC pinpoint 2.0+
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
6. export COLLECTOR_CONFIG=/full path of collector.conf/
7. run collector-agent
    ```
    $ ./init_python_env.sh
    $ python run.py 
    ```

### [How To Use]
[Click me ☚](../../Example/PY/Readme.md)


## Changes

