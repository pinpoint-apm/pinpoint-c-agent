## Support Plan
What we have supported and what we are going to support: [support plan](SupportPlan.md)

## Getting Started

### Requirement

Dependency| Version| More
---|----|---
python |2.7,3+ | (async must 3.7.1+)
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

[pinpoint-python-plugins.tar.gz](https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/V2020.12.17/pinpoint-python-plugins-v0.0.1.tar.gz)

#### Install Collector Agent
There are two ways to install Collector-Agent, just choose one:

1. [Build Collector-Agent yourself ☚](../CollectorAgent/Readme.md)
2. Use Dockerized Collector-Agent:
    [env.file](../../collector-agent/conf/env.file)
    ```
    docker pull eeliu2020/pinpoint-collector-agent:latest 
    docker run --env-file path-to-env.file -d -p 9999:9999 eeliu2020/pinpoint-collector-agent
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