# How TO Use Pinpoint

## Example Of A Simple Http Server

Refer to [http-server](./simple-http-server/http-server.py) and [BaseHTTPRequestPlugins](./simple-http-server/plugins/BaseHTTPRequestPlugins.py)

> Steps

1. Use preRequestPlugins(BaseHTTPRequestPlugins) to bind the requests handdler function.

```python
    @BaseHTTPRequestPlugins(/whatever name you like(such as project name)/, /the function name/)
    def do_GET(self):
```

2. Use PinpointCommonPlugin to bind the user function/method you care.

```python
    @PinpointCommonPlugin(/the class name/, /the function name/)
    def getAgent(self,headers):
```



## The Python Frameworks pinpoint-c-agent Supports

Follow the Readme.md under each framework to enable the plugins.

1. [Django](./django) (https://www.djangoproject.com/) 
2. [Flask](./flask) (https://github.com/pallets/flask) 
3. [Tornado](./tornado) (https://www.tornadoweb.org/en/stable/index.html#) 


