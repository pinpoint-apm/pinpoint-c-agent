# Integrating Pinpoint Into Django


## Integrating pinpoint

> Make sure pinpointPy module has been installed. ([How to Install pinpointPy module](../../../DOC/PY/Readme.md))
1. Copy ```pinpoint``` directory to your project root. Add ```PinPointMiddleWare``` into ```MIDDLEWARE``` in Django's setting file.
    >setting.py

    ```
    .....
    MIDDLEWARE = [
        'pinpoint.PinPointMiddleWare.PinPointMiddleWare',
        ......
    ]
    ```
2. Copy [plugins](../plugins) to the ```pinpoint``` directory. Plugins in ```plugin``` are some examples, you can also write your own plugin according to these examples.

3. Hook the function you cared.

     > Example: django/test_recursion.py
     Hook the function ```fact``` by add ```@PinpointCommonPlugin('', __name__)``` just before it.
    
    
    ```
    from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin
    
    
    @PinpointCommonPlugin('', __name__)
    def fact(n):
        ......
    ```

## Demo 


> server map 

![server-map](images/Django-mysql-servermap.png) 

> call stack

![server-stack](images/Django-recursion-calltree.png)

