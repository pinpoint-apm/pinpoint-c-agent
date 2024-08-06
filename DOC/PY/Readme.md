[English](Readme.md) | [中文](Readme-CN.md) | [한국어](Readme-KR.md)


## Getting Started

> Requirement

| Dependency      | Version                                     | More                |
| --------------- | ------------------------------------------- | ------------------- |
| python          | 3+                                          | (async must 3.7.1+) |
| collector-agent | [installed ?](../collector-agent/readme.md) |

### Installation

```shell
$ pip install pinpointPy
```

### Choose your framework


#### 1.1 Flask

> include middleware

```
from pinpointPy import set_agent, monkey_patch_for_pinpoint, use_thread_local_context
use_thread_local_context()
monkey_patch_for_pinpoint()
set_agent("cd.dev.test.py", "cd.dev.test.py",
          'tcp:dev-collector:10000', -1)
app = Flask(__name__)
app.wsgi_app = PinPointMiddleWare(app,app.wsgi_app)
```

#### 1.2 Django

> append into django middleware


settings.py

```python
from pinpointPy import set_agent, monkey_patch_for_pinpoint, use_thread_local_context
use_thread_local_context()
monkey_patch_for_pinpoint()
set_agent("cd.dev.test.py", "cd.dev.test.py",
          'tcp:dev-collector:10000', -1)

MIDDLEWARE = [
    'pinpointPy.Django.DjangoMiddleWare',
    ...
    ]

```
#### 1.3 Fastapi
Settings in app/main.py:
```
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
set_agent("cd.dev.test.py", "cd.dev.test.py",0, 'tcp:dev-collector:10000')
use_starlette_context()
## patch for synchronous libraries, such as requests,myql-connector-python ...
# support lists https://github.com/pinpoint-apm/pinpoint-c-agent/tree/dev/plugins/PY/pinpointPy/libs
monkey_patch_for_pinpoint()
async_monkey_patch_for_pinpoint()
```

#### 1.4 Tornado

Todo....

#### 1.5 pyramid

> Add pinpoint_tween middleware

```python

    config = Configurator(settings=settings, root_factory="conduit.auth.RootFactory")
    config.add_tween('pinpoint.pyramid.tween.pinpoint_tween')

```

~Example [pinpoint-in-pyramid](https://github.com/eeliu/pinpoint-in-pyramid)~

#### 1.6 bottle

> Loading pinpint.Bottle before application run

~Example [pinpoint-in-bottle](https://github.com/eeliu/pinpoint-in-bottle)~

#### 1.7 aiohttp

Todo....

#### 1.8 web.py

> Add pinpoint middleware

~Example [py-web](https://github.com/eeliu/pinpoint-in-pyweb)~


#### 1.9 web2.py (synchronize)

> Add pinpoint middleware

~ Example [py-web2](https://github.com/eeliu/pinpoint-in-pyweb2)~


## Performance Test Result

### Case: flask/test_mysql

| -          | TPR(ms) | RPS(#/sec) |
| ---------- | ------- | ---------- |
| pinpointPy | 4.487   | 445.73     |
| -          | 4.498   | 444.69     |
| -          | 4.526   | 441.88     |
| pure       | 4.440   | 450.44     |
| -          | 4.479   | 446.51     |
| -          | 4.425   | 451.96     |
| Result     | +0.05ms | -1%        |

> TPR: time per request         
> RPS: requests per second
