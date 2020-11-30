## Support Plan
What we have supported and what we are going to support: [support plan](SupportPlan.md)

## Getting Started

### Requirement

Dependency| Version
---|----
python |python 3.5+
python async|python 3.7.1+
gcc|gcc 4.7+
cmake| 3.0+
*inux| 
pinpoint| 2.0+(GRPC)

### Installation

#### Build pinpointPy module

1. Install python virtual environment, and please refer to 
https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/

2. Install pinpointPy (Python virtual environment is recommended).
```shell
$ pip install pinpointPy
```
#### Install Collector Agent
There are two ways to install Collector-Agent, just choose one:

1. [Build Collector-Agent yourself ☚](../CollectorAgent/Readme.md)
2. Use Dockerized Collector-Agent:
    
    ```
    docker pull eeliu2020/pinpoint-collector-agent:latest 
    docker run  --add-host collectorHost:your-pinpoint-hostname -d -p 9999:9999 eeliu2020/pinpoint-collector-agent
    ```
   

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