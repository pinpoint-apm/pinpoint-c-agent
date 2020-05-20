# Integrating Pinpoint Into Tornado


## Integrating pinpoint

> Make sure pinpointPy module has been installed. ([How to Install pinpointPy module](../../../DOC/PY/Readme.md))
1. Copy [plugins](../tornado/plugins) to your root. Plugins in tornado/plugins are some examples, you can also write your own plugin according to these examples.

2. Use preRequestPlugins(TornadoHTTPRequestPlugins) to bind the requests handler function.
    ```python
    class MainHandler(tornado.web.RequestHandler):
        @TornadoHTTPRequestPlugins('tornado.web.RequestHandler', __name__)
        async def get(self):
    ```

3. Use PinpointCommonPlugin to bind the user-defined function/method you care.
    ```python
        @PinpointCommonPlugin('',__name__)
        async def get_1(self,t = 2):
    ```
