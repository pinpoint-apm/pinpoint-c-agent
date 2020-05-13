# Integration pinpoint into Django.


## Integration pinpoint

> Make sure pinpointPy module has been installed. ([How to Install pinpointPy module](DOC/PY/Readme.md))
1. Copy [MyMiddleware.py](TestDemo/MyMiddleware.py) to your project. Add ```MyMiddleware``` into ```MIDDLEWARE``` in Django's setting file.
    >setting.py

    ```
    MIDDLEWARE = [
        '/path to MyMiddleware/ MyMiddleware.MyMiddleware',
        ......
    ]
    ```
2. cp plugins(Both plugins in ```plugin``` and ```django/plugins```) to your root

3. Hook the function you want to monitor.

     > Example: django/test_recursion.py
     Hook the function ```fact``` by add ```@PinpointCommonPlugin('', __name__)``` just before it.
    
    
    ```
    from plugins.PinpointCommonPlugin import PinpointCommonPlugin
    
    
    @PinpointCommonPlugin('', __name__)
    def fact(n):
        if n==1:
            return 1
        return n * fact(n - 1)
    ```

## Demo 


> server map 

![server-map](images/Django-mysql-servermap.png) 

> call stack

![server-stack](images/Django-recursion-calltree.png)


## References
1. How to install yii? [ click me ☚ ](https://www.yiiframework.com/doc/guide/2.0/en/start-installation)
2. What's php_simple_aop? [ click me ☚ ](https://github.com/eeliu/php_simple_aop)