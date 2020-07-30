# How to Use Pinpoint？

## PinpointPy connect to Collector-agent
Both sock and tcp supports, you could config in [PinpointCommon.py](plugins/PinpointCommon.py)

Please make sure PinpointPy use the same with Collector-agent.
> sock

```
pinpointPy.set_agent(collector_host=COLLECTOR_HOST,trace_limit=-1)
```
> TCP

```
pinpointPy.set_agent(collector_host='Tcp:ip:port',trace_limit=-1)
```

## Example of A Simple Http Server

Refer to [http-server](https://github.com/naver/pinpoint-c-agent/blob/v0.3.1/Example/PY/simple-http-server/http-server.py) and [BaseHTTPRequestPlugins](https://github.com/naver/pinpoint-c-agent/blob/v0.3.1/Example/PY/simple-http-server/plugins/BaseHTTPRequestPlugins.py)

> Steps

1. Use preRequestPlugins(BaseHTTPRequestPlugins) to bind the requests handler function.

```python
    @BaseHTTPRequestPlugins(/whatever name you like(such as project name)/, /the function name/)
    def do_GET(self):
```

2. Use PinpointCommonPlugin to bind the user-defined function/method you care.

```python
    @PinpointCommonPlugin(/the class name/, /the function name/)
    def getAgent(self,headers):
```

## The Library pinpoint-py Supports.

1. [PyMysql](./plugins/PyMysql)
2. [requests](./plugins/requests)

#### How to Use
> Example: Refer to [PyMysql/\_\_init\_\_.py](./plugins/PyMysql/__init__.py)

> Steps

1. Add the function you care into ```HookSet``` in ```plugins/PyMysql/__init__.py```:
    ```python
    HookSet = [
        ('pymysql', 'connect', pymysql, pymysql.connect),
        ('Cursor','execute', Cursor, Cursor.execute),
        ('Cursor','fetchall', Cursor, Cursor.fetchall)
    ]
    ```
2. Import it in ```plugin/__init__.py```
    ```python
    from plugins.PyMysql import *
    ```

## The Python Frameworks pinpoint-py Supports

Follow the Readme.md under each framework to enable the plugins.

1. [Django](./django) (https://www.djangoproject.com/) 
2. [Flask](./flask) (https://github.com/pallets/flask) 
3. [Tornado](./tornado) (https://www.tornadoweb.org/en/stable/index.html#) 


