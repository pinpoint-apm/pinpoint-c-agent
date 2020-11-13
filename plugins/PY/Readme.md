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
    'pinpoint.DjangoMiddleWare',
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


### 2. Specify pinpoint option into settings.py

