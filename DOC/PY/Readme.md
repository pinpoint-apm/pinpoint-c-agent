# Tutorial for pinpoint-py agent

## Getting Started

> Requirement

Dependency| Version| More
---|----|---
python |2.7,3+ | (async must 3.7.1+)
GO | | 
gcc|gcc 4.7+| c++11
cmake| 3.1+| 
*inux|  | `windows` is on the way
pinpoint| 2.0+(GRPC)|
collector-agent| [installed ?](../collector-agent/readme.md)

### Installation

```shell
$ pip install pinpointPy
```

### How to Use


#### 1.1 Flask

> include middleware

```
app = Flask(__name__)
app.wsgi_app = PinPointMiddleWare(app,app.wsgi_app)
```

#### 1.2 Django

> append into django middleware


settings.py

```python

MIDDLEWARE = [
    'pinpointPy.Django.DjangoMiddleWare',
    ...
    ]

```
#### 1.3 Fastapi
Settings in app/main.py:
```
# pinpoint
##############################################
from starlette_context.middleware import ContextMiddleware
from starlette_context import context, plugins

from pinpointPy import set_agent
from pinpointPy.Fastapi import asyn_monkey_patch_for_pinpoint
from starlette.middleware import Middleware
from pinpointPy.Fastapi import PinPointMiddleWare

middleware = [
    Middleware(ContextMiddleware),
    Middleware(PinPointMiddleWare)
]
asyn_monkey_patch_for_pinpoint()
set_agent("fastapi-redis", "fastapi-redis", 'tcp:10.34.130.152:9999', -1, True)
##############################################
```
Example: [fastapi-redis-pinpoint](https://github.com/EyelynSu/fastapi-redis-pinpoint)

#### 1.3 Tornado

Todo....

#### 1.4 pyramid

> Add pinpoint_tween middleware

```python

    config = Configurator(settings=settings, root_factory="conduit.auth.RootFactory")
    config.add_tween('pinpoint.pyramid.tween.pinpoint_tween')

```

Example [pinpoint-in-pyramid](https://github.com/eeliu/pinpoint-in-pyramid)

#### 1.5 bottle

> Loading pinpint.Bottle before application run

Example [pinpoint-in-bottle](https://github.com/eeliu/pinpoint-in-bottle)

#### 1.6 aiohttp

Todo....

#### 1.7 web.py

> Add pinpoint middleware

> Example [py-web](https://github.com/eeliu/pinpoint-in-pyweb)


#### 1.8 web2.py (synchronize)

> Add pinpoint middleware

> Example [py-web2](https://github.com/eeliu/pinpoint-in-pyweb2)


### 2. Configuration


```py

# enable auto-interceptor plugins
monkey_patch_for_pinpoint()
# set pinpoint related environment
set_agent("flask-agent","FLASK-AGENT",'tcp:dev-collector:9999',-1)

```


## Performance Test Result

### Case: flask/test_mysql

-|TPR(ms)|RPS(#/sec)
----|-----|----
pinpointPy|4.487|445.73|
-|4.498 |444.69
-|4.526 |441.88
pure|4.440|450.44
-|4.479|446.51
-|4.425|451.96
Result|+0.05ms|-1%

> TPR: time per request         
> RPS: requests per second
