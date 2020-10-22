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
from pinpoint.TornadoPlugin import HTTPRequestPlugins as PinpointHTTPRequestPlugins,\
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


### 2. Specify pinpoint option into settings.py

