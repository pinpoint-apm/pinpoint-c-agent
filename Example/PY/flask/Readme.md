# Integration Pinpoint Into Flask.


## Integration pinpoint

> Make sure pinpointPy module has been installed. ([How to Install pinpointPy module](../../../DOC/PY/Readme.md))
1. Copy [PinPointMiddleWare.py](PinPointMiddleWare.py) to your project. Add PinPointMiddleWare to your application.

    ```
    app = Flask(__name__)
    
    from PinPointMiddleWare import PinPointMiddleWare
    app.wsgi_app = PinPointMiddleWare(app,app.wsgi_app)
    ......
    ```
2. Copy plugins(Both ```plugin``` and ```flask/plugins```) to your root. Plugins in ```plugin``` and ```flask/plugins``` are some examples, you can also write your own plugin according to these examples.

3. Hook the function you cared.

     > Example: flask/test_recursion.py
     Hook the function ```fact``` by add ```@PinpointCommonPlugin('', __name__)``` just before it.
    
    
    ```
    from plugins.PinpointCommonPlugin import PinpointCommonPlugin
    
    
    @PinpointCommonPlugin('', __name__)
    def fact(n):
        ......
    ```

