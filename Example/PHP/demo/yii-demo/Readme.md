# Yii， php agent supported! 

> Note: php-agent rewrites the classloader of Yii framework. Therefore, you should follow below Steps.


## Steps

1. Replace the Yii 'web/index.php' with our 'web/index.php'
2. cp plugins to your root and enable autoload

 > composer.json

``` 
  "autoload": {
        "psr-4": {
            "Plugins\\": "Plugins/"
        }
    },
```

3. Add requirement

 > composer.json

```
 "require": {
     ...
     "eeliu/php_simple_aop": "v0.2.4"
     ...
 }


```

4. Make it works
   
``` shell
    $ composer install (or composer update )
```


## Demo 


> server map 

![server-map](images/yii-server-map.png) 

> call stack

![server-stack](images/yii-call-stack.png)


## References
1. How to install yii? [ click me ☚ ](https://www.yiiframework.com/doc/guide/2.0/en/start-installation)
2. What's php_simple_aop? [ click me ☚ ](https://github.com/eeliu/php_simple_aop)