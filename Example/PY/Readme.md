# How to Use Pinpoint？

## Example Of A Simple Http Server

Refer to [http-server](./simple-http-server/http-server.py) and [BaseHTTPRequestPlugins](./simple-http-server/plugins/BaseHTTPRequestPlugins.py)

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

#### How To Use
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

