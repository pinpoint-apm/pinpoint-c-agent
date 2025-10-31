[English](Readme.md) | [中文](Readme-CN.md) | [한국어](Readme-KR.md)

# pinpoint-py agent 가이드

## 시작 가이드

### 요구

| 디펜던시        | 버전                                        |                     |
| --------------- | ------------------------------------------- |
| python          | 3+                                          | (async must 3.7.1+) |
| collector-agent | [installed ?](../collector-agent/Readme.md) |

### 설치 스텝 

#### pinpointPy 모듈 구성 

```shell
$ pip install pinpointPy==1.3.1
```

### 프레임워크 인티그레이션 


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

from pinpointPy import set_agent, monkey_patch_for_pinpoint, use_thread_local_context, use_asyncio_local_context
from pinpointPy.Fastapi import asyn_monkey_patch_for_pinpoint
from starlette.middleware import Middleware
from pinpointPy.Fastapi import PinPointMiddleWare

middleware = [
    Middleware(ContextMiddleware),
    Middleware(PinPointMiddleWare)
]
set_agent("cd.dev.test.py", "cd.dev.test.py",0, 'tcp:dev-collector:10000')
use_thread_local_context()
use_asyncio_local_context()
## patch for synchronous libraries, such as requests,myql-connector-python ...
# support lists https://github.com/pinpoint-apm/pinpoint-c-agent/tree/dev/plugins/PY/pinpointPy/libs
monkey_patch_for_pinpoint()
asyn_monkey_patch_for_pinpoint()
```

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

### 2. 컨피그레이션

```py

# enable auto-interceptor plugins
monkey_patch_for_pinpoint()
# set pinpoint related environment
set_agent("flask-agent","FLASK-AGENT",'tcp:dev-collector:9999',-1)

```

## 프로파일링 결과 

### Case: flask/test_mysql

| -           | TPR(ms) | RPS(#/sec) |
| ----------- | ------- | ---------- |
| pinpoint-py | 4.487   | 445.73     |
| -           | 4.498   | 444.69     |
| -           | 4.526   | 441.88     |
| pure        | 4.440   | 450.44     |
| -           | 4.479   | 446.51     |
| -           | 4.425   | 451.96     |
| Result      | +0.05ms | -1%        |

> TPR: time per request         
> RPS: requests per second
