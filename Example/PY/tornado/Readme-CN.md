# 将pinpoint集成到Tornado中


## 集成Pinpoint

> 确保安装了pinpointPy模块。([如何安装pinpointPy模块](../../../DOC/PY/Readme.md))
1. 将[插件](../tornado/plugins) 复制到您的根路径下， ```plugin``` 里的插件是一些示例，您也可以根据这些例子编写您自己的插件。

2. 用 preRequestPlugins(TornadoHTTPRequestPlugins) 来绑定处理器函数上的请求。
    ```python
    class MainHandler(tornado.web.RequestHandler):
        @TornadoHTTPRequestPlugins('tornado.web.RequestHandler', __name__)
        async def get(self):
    ```

3. 用 PinpointCommonPlugin 来绑定您所关心的用户自定义的函数/方法。
    ```python
        @PinpointCommonPlugin('',__name__)
        async def get_1(self,t = 2):
    ```
