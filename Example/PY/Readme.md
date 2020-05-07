## Getting Started

### Requirement

Dependency|Version
---|----
PYTHON | 3.5+
GCC| GCC 5+
cmake| 3.0+
*inux|
pinpoint| 1.8.0-RC1(thrift) <br> 2.0+(GRPC)

### Installation
#### install python virtual environment 

https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/

#### Install pinpoint-py module

```
$ cd PY/pinpoint-py-module
$ sh setup_all_in_one.sh
$ python setup.py install
```
#### integration pinpoint into your project

> Steps

1. Binding the preRequestPlugins

``` python
    @BaseHTTPRequestPlugins('SimpleWebServer',__name__)
    def do_GET(self):
```

2. Binding the function/method you care

```
    @PinpointCommonPlugin('SimpleWebServer',__name__)
    def getAgent(self,headers):
```



### TODO


pip install --extra-index-url https://test.pypi.org/simple/ pinpointPy