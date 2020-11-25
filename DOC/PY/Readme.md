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

2. Come back to the root directory, and install pinpointPy (Python virtual environment is recommended).
```shell
$ python setup.py install
```
#### Build Collector Agent
[Collector Agent Installation ☚](../CollectorAgent/Readme.md)

### [How to Use]
[Click me ☚](../../Example/PY/Readme.md)


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