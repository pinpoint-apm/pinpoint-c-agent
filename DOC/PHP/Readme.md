## Support Plan
What we have supported and what will support: [support plan](SupportPlan.md)

## Getting Started

### Requirement

Dependency|Version| More
---|----|----
PHP| php `7+` ,`5+`|
GO | | 
GCC| GCC `4.7+`| C++11 
cmake| cmake `3.2+`|
*inux|| `windows is on the way`
pinpoint| `2.0+`|
composer| | class can be automatic pinpoint-cut

### Installation

#### Steps
1. git clone https://github.com/pinpoint-apm/pinpoint-c-agent.git
   
2. Goto collector-agent(`pinpoint-c-agent/collector-agent`) and build `Collector-Agent`

    `Collector-Agent`, who formats the span from PHP/Python/C/CPP-Agent and send to `Pinpoint-Collector`, is an agent written by [golang](https://golang.google.cn/).Please install golang before the following steps.[Install GO](https://golang.google.cn/doc/install)
   1. Execute command `go build`
   2. Add environment variables:
      ```
        export PP_COLLECTOR_AGENT_SPAN_IP=dev-pinpoint
        export PP_COLLECTOR_AGENT_SPAN_PORT=9993
        export PP_COLLECTOR_AGENT_AGENT_IP=dev-pinpoint
        export PP_COLLECTOR_AGENT_AGENT_PORT=9991
        export PP_COLLECTOR_AGENT_STAT_IP=dev-pinpoint
        export PP_COLLECTOR_AGENT_STAT_PORT=9992
        export PP_COLLECTOR_AGENT_ISDOCKER=false
        export PP_LOG_DIR=/tmp/
        export PP_Log_Level=INFO
        export PP_ADDRESS=0.0.0.0@9999
      ```
      1. `PP_COLLECTOR_AGENT_SPAN_IP`, `PP_COLLECTOR_AGENT_AGENT_IP`, `PP_COLLECTOR_AGENT_STAT_IP`: Set the IP of pinpoint-collector.
      2. `PP_COLLECTOR_AGENT_SPAN_PORT`, `PP_COLLECTOR_AGENT_AGENT_PORT`, `PP_COLLECTOR_AGENT_STAT_PORT`: Set the port of pinpoint-collector(grpc).
      3. `PP_LOG_DIR`: Set the path of Collector-Agent's log file.
      4. `PP_Log_Level`: Set the log level.
      5. `PP_ADDRESS`: Set the address of `Collector-Agent`, then `PHP/Python-Agent` will connect Collector-Agent through this address.
   3. Run `Collector-Agent` by executing command `./CollectorAgent`
   
   Collector Agent Span Specification
   [Json string map to Pinpoint item](../API/collector-agent/Readme.md)
   
3. Build pinpoint_php module, goto the root directory of pinpoint-c-agent installation package, and do following steps:
   1. phpize        
   2. ./configure
   3. make 
   4. make test TESTS=src/PHP/tests/ 
   
        ps: We recommend you to test whether the module is RIGHT. For PHP5: ```make test TESTS=src/PHP/tests5/```
   5. make install 
   6. Activate pinpoint-php-module, please add the following configuration into your ```php.ini``` 
   
       >  php.ini 
        ```ini
        extension=pinpoint_php.so
        ; Collector-agent's TCP address, ip,port:Collector-Agent's ip,port, please ensure it consistent with the `PP_ADDRESS` of `Collector-Agent` in step2(Build Collector-Agent).
        pinpoint_php.CollectorHost=Tcp:ip:port
        pinpoint_php.SendSpanTimeOutMs=0 # 0 is recommanded
        ; request should be captured duing 1 second. < 0 means no limited
        pinpoint_php.TraceLimit=-1 
        ; DEBUG the agent, PHP's log_error should turn on too.
        ;pinpoint_php.DebugReport=true
        ;error_reporting = E_ALL
        ;log_errors = On
        ;error_log = /tmp/php_fpm_error.log
        ```

4. Use Pinpoint PHP-Agent in your project, and follow the steps below: 
    
    We assume that you have installed composer and known how to use it. [How to Use Composer?](https://getcomposer.org/doc/00-intro.md)

   1. Add ```pinpoint-apm/pinpoint-php-aop``` into composer.json and update.
        ```
        "require": {
            ...
            "pinpoint-apm/pinpoint-php-aop": "dev-master"
        }
        ```
        or
        ```
        composer require pinpoint-apm/pinpoint-php-aop
        ```
   2. Add the following constants in the index file of your project:
   
        ```
        #################################################
        define('APPLICATION_NAME','APP-2');
        define('APPLICATION_ID','app-2');
        define('AOP_CACHE_DIR',__DIR__.'./Cache/');
        define('PLUGINS_DIR',__DIR__.'./Plugins/');
        define('PINPOINT_USE_CACHE','YES');
        require_once __DIR__. path to 'vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
        #################################################
        ```
        1. ```APPLICATION_NAME```: Application name.
        2. ```APPLICATION_ID```: Agent ID.
        3. ```AOP_CACHE_DIR```: Where to generate ```Cache```.
        4. ```PLUGINS_DIR```: Path to ```Plugins```.
        5. ```PINPOINT_USE_CACHE```: 'YES' will not update ```Cache``` when request coming; 'No' will update ```Cache``` when every request coming.(You can also update ```Cache``` by just deleting it.) Considering the performance, we recommend 'YES'. Further more, if you modify the plugins, you should update the ```Cache``` to take effect.
        6. ```require_once __DIR__. path to 'vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';```: Require pinpoint's ```auto_pinpointed.php```.**Please add after ```require_once __DIR__."/../vendor/autoload.php";```, this is very important!**

    3. Choose you framework and copy the directory as `Plugins` to the root of your application, autoload ```Plugins``` in ```composer.json```.
        Details for frameworks:
            [ThinkPHP5](../../plugins/PHP/Framework/ThinkPHP5)
            [Yii2](../../plugins/PHP/Framework/Yii2)
            [laravel](../../plugins/PHP/Framework/laravel)

        > composer.json
        ```
        "autoload": {
                "psr-4": {
                    ......
                    "Plugins\\": path to the Plugins
                }
            },
        ```
        
        We have prepared some examples for you, please goto [testapps](../../testapps/PHP).


## Changes 

- Fully support ZTS
- More stability 
- Higher performance 
- Easily use and maintain
- Support GRPC

## F & Q

### 1. How to regenerate all AOP files?

 Delete *__class_index_table* in Cache/

### 2. Why not support automatically update AOP files?

We can DO but prefer not to DO! Because we have not yet found an efficient way to implement this and monitoring these files status every time is a bad deal.

### 3. How much performance does it lose when using?

After full test, the AOP code could spend at least 1ms in our env(E5-2660 v4 @ 2.00GHz). While, the more function/method you monitor, the more you take.

#### Performance Test Result

##### 3.1 [Test Result ☚](./detail_versions.md#performance-loss-under-stress-test)

##### 3.2 [Flarum Test Result ☚](./User%20Manual.md#1.1-performance-result)

### For the developer

[Plugins Tutorial ☚](../../plugins/PHP/Readme.md)

[PHP user manual ☚](./User%20Manual.md)

### Known Issues

#### 1. Exception or error message with a wrong file path.

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/99

#### 2. If I do not use composer/autoloader, can I use this version?

Sorry, `pinpoint-php-aop` does not support to wrap the user class (or internal class) without composer/autoloader. By the way, [Composer](https://getcomposer.org/) is GOOD. O(∩_∩)O

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/103

#### 3. ~Why not support PHP5.x.~ Already done!

https://www.php.net/supported-versions.php

#### 4. ~Generator function is not supported.~ Already done!

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/100

#### 5. ~Some built-in functions can't AOP~ Already done!
https://github.com/pinpoint-apm/pinpoint-c-agent/issues/102
