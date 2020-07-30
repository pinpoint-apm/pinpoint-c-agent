# Integrating Pinpoint Into Flask


## Integrating pinpoint

> Make sure pinpointPy module has been installed. ([How to Install pinpointPy module](../../../DOC/PY/Readme.md))
1. Copy ```pinpoint``` directory to your project root. Add PinPointMiddleWare to your application.

    ```
    app = Flask(__name__)
    
    from PinPointMiddleWare import PinPointMiddleWare
    app.wsgi_app = PinPointMiddleWare(app,app.wsgi_app)
    ......
    ```
2. Copy [plugins](../flask/pinpoint/plugins) to the ```pinpoint``` directory . Plugins in ```plugin``` are some examples, you can also write your own plugin according to these examples.

3. Hook the function you cared.

     > Example: flask/test_recursion.py
     Hook the function ```fact``` by add ```@PinpointCommonPlugin('', __name__)``` just before it.
    
    
    ```
    from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin
    
    
    @PinpointCommonPlugin('', __name__)
    def fact(n):
        ......
    ```

