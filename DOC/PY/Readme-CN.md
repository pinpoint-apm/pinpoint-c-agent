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
#### 搭建 Collector Agent
[Collector Agent 安装步骤 ☚](../CollectorAgent/Readme-CN.md)

### [如何使用]
[请点击 ☚](../../Example/PY/Readme-CN.md)


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