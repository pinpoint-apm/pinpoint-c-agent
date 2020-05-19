# 如何使用pinpoint？

## PinpointPy 与 Collector-agent 的连接
支持 sock 和 tcp 连接, 可以在 [PinpointCommon.py](plugins/PinpointCommon.py) 中配置。

请确保 PinpointPy 和 Collector-agent 使用的同样的连接。
> sock

```
pinpointPy.set_agent(collector_host=COLLECTOR_HOST,trace_limit=-1)
```
> TCP

```
pinpointPy.set_agent(collector_host='Tcp:ip:port',trace_limit=-1)
```

## 一个简单的 Http Server 的例子

请参考 [http-server](./simple-http-server/http-server.py) 和 [BaseHTTPRequestPlugins](./simple-http-server/plugins/BaseHTTPRequestPlugins.py)

> 步骤

1. 用 preRequestPlugins(BaseHTTPRequestPlugins) 来绑定处理器函数上的请求。

```python
    @BaseHTTPRequestPlugins(/whatever name you like(such as project name)/, /the function name/)
    def do_GET(self):
```

2. 用 PinpointCommonPlugin 来绑定您所关心的用户自定义的函数/方法。

```python
    @PinpointCommonPlugin(/the class name/, /the function name/)
    def getAgent(self,headers):
```

## pinpoint-py支持的库

1. [PyMysql](./plugins/PyMysql)
2. [requests](./plugins/requests)

#### 如何使用
> 示例: 请参考 [PyMysql/\_\_init\_\_.py](./plugins/PyMysql/__init__.py)

> 步骤

1. 在```plugins/PyMysql/__init__.py```中，将您所关心的函数添加到```HookSet``` :
    ```python
    HookSet = [
        ('pymysql', 'connect', pymysql, pymysql.connect),
        ('Cursor','execute', Cursor, Cursor.execute),
        ('Cursor','fetchall', Cursor, Cursor.fetchall)
    ]
    ```
2. 将它导入到 ```plugin/__init__.py```中
    ```python
    from plugins.PyMysql import *
    ```

## pinpoint-py 支持的Python框架

遵照Readme.md，在每个框架下启用插件。

1. [Django](./django) (https://www.djangoproject.com/) 
2. [Flask](./flask) (https://github.com/pallets/flask) 
3. [Tornado](./tornado) (https://www.tornadoweb.org/en/stable/index.html#) 


