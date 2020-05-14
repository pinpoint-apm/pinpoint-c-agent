# Integration Pinpoint Into Django.


## Integration pinpoint

> Make sure pinpointPy module has been installed. ([How to Install pinpointPy module](../../../DOC/PY/Readme.md))
1. Copy [PinPointMiddleWare.py](TestDemo/PinPointMiddleWare.py) to your project. Add ```PinPointMiddleWare``` into ```MIDDLEWARE``` in Django's setting file.
    >setting.py

    ```
    .....
    MIDDLEWARE = [
        '/path to PinPointMiddleWare/ PinPointMiddleWare.MyMiddleware',
        ......
    ]
    ```
2. Copy plugins(Both ```plugin``` and ```django/plugins```) to your root. Plugins in ```plugin``` and ```django/plugins``` are some examples, you can also write your own plugin according to these examples.

3. Hook the function you cared.

     > Example: django/test_recursion.py
     Hook the function ```fact``` by add ```@PinpointCommonPlugin('', __name__)``` just before it.
    
    
    ```
    from plugins.PinpointCommonPlugin import PinpointCommonPlugin
    
    
    @PinpointCommonPlugin('', __name__)
    def fact(n):
        ......
    ```

## Demo 


> server map 

![server-map](images/Django-mysql-servermap.png) 

> call stack

![server-stack](images/Django-recursion-calltree.png)

