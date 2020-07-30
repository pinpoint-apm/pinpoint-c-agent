# 将pinpoint集成到Flask中


## 集成Pinpoint

> 确保安装了pinpointPy模块。([如何安装pinpointPy模块](../../../DOC/PY/Readme-CN.md))
1. 将```pinpoint``` 目录复制到您项目的根目录， 将 PinPointMiddleWare 添加到您的应用程序中。

    ```
    app = Flask(__name__)
    
    from PinPointMiddleWare import PinPointMiddleWare
    app.wsgi_app = PinPointMiddleWare(app,app.wsgi_app)
    ......
    ```
2. 将[插件](../flask/pinpoint/plugins)复制到```pinpoint``` 目录。 ```plugin``` 里的插件是一些示例，您也可以根据这些例子编写您自己的插件。

3. Hook 您所关心的函数。

     > Example: flask/test_recursion.py
     Hook the function ```fact``` by add ```@PinpointCommonPlugin('', __name__)``` just before it.
    
    
    ```
    from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin
    
    
    @PinpointCommonPlugin('', __name__)
    def fact(n):
        ......
    ```

