## How to use 
### 1.Import your framework's entry file

#### 1.1 Flask

```python
import pinpoint.FlaskPlugins.*
```

#### 1.2 Django

> append into django middleware


settings.py

```python

MIDDLEWARE = [
    'pinpoint.Django.DjangoMiddleWare',
    ...
    ]

```
#### 1.3 Tornado

```python
from pinpoint.TornadoPlugin import RequestPlugin as PinpointHTTPRequestPlugins,\
    CommonPlugin as PinpointCommonPlugin

class MainHandler(tornado.web.RequestHandler):
    @PinpointHTTPRequestPlugins('tornado.web.RequestHandler', __name__)
    async def get(self):
        await self.get_1()
        self.write("Hello, world")

    @PinpointCommonPlugin('',__name__)
    async def get_1(self,t = 2):
        await gen.sleep(0.01)
        if t == 0:
            return
        else:
            t-=1
            await self.get_1(t)
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


#### 1.8 web2.py (syn)

> Add pinpoint middleware

> Example [py-web2](https://github.com/eeliu/pinpoint-in-pyweb2)


### 2. Specify pinpoint option into [settings.py](./pinpoint/settings.py)

```
###############################################################
# user should set below before use
APP_ID ='python-agent' # application id
APP_NAME ='PYTHON-AGNNT' # application name
# COLLECTOR_HOST='unix:/tmp/collector-agent.sock'
COLLECTOR_HOST='tcp:dev-collector:9999'
APP_SUB_ID='1'
###############################################################

pinpointPy.set_agent(collector_host=COLLECTOR_HOST,trace_limit=-1)

# pinpointPy.set_agent(collector_host='Tcp:ip:port',trace_limit=-1)

# def output(msg):
#     print(msg)
#
pinpointPy.enable_debug(None)
```
1. APP_ID: Agent ID.
2. APP_NAME: Application name.
3. COLLECTOR_HOST: Collector-Agent's address.
4. pinpointPy.enable_debug(None): DEBUG agent, pinpoint infomation will print in concole. Commented out it, pinpoint infomation will not be printed. For performance, recommend commented out it.
