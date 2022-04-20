# pinpoint-py agent 使用说明

## 入门指南

### 要求

依赖| 版本
---|----
python |python 3.5+
python async|python 3.7.1+
GO | | 
gcc|gcc 4.7+
cmake| 3.0+
*inux| 
pinpoint| 2.0+(GRPC)
collector-agent| [installed ?](../collector-agent/readme.md)

### 安装步骤

#### 搭建 pinpointPy 模块

```shell
$ pip install pinpointPy
```

### 框架集成


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

### 2. 配置

```py

# enable auto-interceptor plugins
monkey_patch_for_pinpoint()
# set pinpoint related environment
set_agent("flask-agent","FLASK-AGENT",'tcp:dev-collector:9999',-1)

```

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