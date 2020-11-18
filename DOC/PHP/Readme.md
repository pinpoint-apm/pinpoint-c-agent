## 1 Getting Started

### Requirement

Dependency|Version
---|----
PHP| php `7+` ,`5+`
GCC| GCC `4.7+`
*inux|
Python | `Python 3`
pinpoint| `1.8.0+`, `2.0+`
**Composer**| 

### Installation

#### Steps
1. git clone https://github.com/naver/pinpoint-c-agent.git
   
> Build pinpoint-php-module

2. Goto your root directory of pinpoint-c-agent installation package 
   1. phpize        
   2. ./configure
   3. make 
   4. make test TESTS=src/PHP/tests/ # We recommend you test whether the module is RIGHT.
   5. make install 
   6. Activation pinpoint module # Add the following to the configuration file in php/lib/php.ini 
   
       >  php.ini 
        ```ini
        extension=pinpoint_php.so
        # unix:(unix sock address)
        pinpoint_php.CollectorHost=unix:/tmp/collector-agent.sock
        # or TCP address
        #pinpoint_php.CollectorHost=Tcp:ip:port
        pinpoint_php.SendSpanTimeOutMs=0 # 0 is recommanded
        # request should be captured duing 1 second. < 0 means no limited
        pinpoint_php.TraceLimit=-1 
        ```
> Build Collector-agent
3. Goto collector-agent
   1. python3 -m venv env
   2. source env/bin/activate
   3. pip3 install -r requirements.txt
   4. Change configuration file in conf/collector.conf

        ```ini
        [Collector]
        # collectorhost and specific port
        ### For thrift  pinpoint 1.8.0-RC1+
        #CollectorSpanIp=collectorHost
        #CollectorSpanPort=9905
        #CollectorStatIp=collectorHost
        #CollectorStatPort=9906
        #CollectorTcpIp=collectorHost
        #CollectorTcpPort=9907
        ### For GRPC  pinpoint 2.0+
        AgentID=your_id
        ApplicationName=your_name
        collector.grpc.agent.ip=collectorHost
        collector.grpc.agent.port=9991
        collector.grpc.stat.ip=collectorHost
        collector.grpc.stat.port=9992
        collector.grpc.span.ip=collectorHost
        collector.grpc.span.port=9993
        [Common]
        # your web server (nginx&apache) port
        Web_Port=8001   
        # debug in dev
        Log_Level=ERROR 
        # make sure LOG_DIR is exist
        # The real-time log of collector-agent can be checked under this path with the command " tail -f ".
        LOG_DIR=/your log dir/ 
        [Agent]
        # the same as below "pinpoint_php.CollectorHost"
        # sock address
        Address=/tmp/collector-agent.sock
        # or TCP address
        #Address=ip@port
        ```
    5. export COLLECTOR_CONFIG=/full path of collector.conf/
    6. run  collector-agent
        ```
        $ ./init_python_env.sh
        $ python run.py 
        ```
   
> Try the aop example

4. Goto Example/PHP (If you use PHP5, goto Example/PHP5)
   1. Download composer.phar. [How to Use Composer?](https://getcomposer.org/doc/00-intro.md)
   2. php composer.phar install
   3. Set $PWD/app/ as your web site root path 



## TODO


## 2 Changes 

- Fully support ZTS
- More stability 
- Higher performance 
- Easily use and maintain
- Support GRPC
- Async API for Swoole-src/workerman/torando

## 3 F & Q

### 3.1 How to regenerate all AOP files?

Delete *__class_index_table* in Cache/

### 3.2 Why not support automatically update AOP files?

We can DO but prefer not DO! Because we have not yet find an efficient way to implement this and monitoring these files status every time is a bad deal.

### 3.3 How much performance does it lose when using?

After full test, the AOP code could cost at least 1ms in our env(E5-2660 v4 @ 2.00GHz). While, the more function/method you monitoring, the more you taking.

## 4 Performance Test Result

### 4.1 [Test Result ☚](./Readme.md#performance-loss-under-stress-test)

### 4.2 [Flarum Test Result ☚](./User%20Manual.md#1.1-performance-result)


### 4.3 Performance Loss Under Stress Test

> Test Scene

```
Test Object: PHPMYAdmin
URL: http://dev-mysql-phpmyadmin/db_structure.php?server=1&db=testDB
Plugins Amount: 4
Hooked Times: 30
```

> Test Result

Case|MTT
---|----
phpmyadmin without pinpoint|387.28ms
phpmyadmin with pinpoint-php v0.1|511.46ms
phpmyadmin with pinpoint-php v0.2|398.26ms


```
MTT: Mean RTT Time 
```
> [ pinpoint-php on Flarum test result ](./User%20Manual.md#1.1-performance-result)

> By the way, pinpoint is an APM system but not a performance enhancement system. The loss can't be avoided at present.


## 5  For the developer

[Plugins Tutorial ☚](../../Example/PHP-Readme-CN.md)

[PHP user manual ☚](./User%20Manual.md)

[v0.1 VS v0.2+](./detail_versions.md)

##6 Known Issues

### 6.1 Exception or error message with a wrong file path.

https://github.com/naver/pinpoint-c-agent/issues/99

### 6.2 If I do not use composer/autoloader, can I use this version?

Sorry, `php-simple-aop` does not support wrapping the user class (or internal class) without composer/autoloader. By the way, [Composer](https://getcomposer.org/) is GOOD. O(∩_∩)O

https://github.com/naver/pinpoint-c-agent/issues/103

### 6.3 ~Why not support PHP5.x.~ Already done!

https://www.php.net/supported-versions.php

### 6.4 ~Generator function is not supported.~ Already done!

https://github.com/naver/pinpoint-c-agent/issues/100

### 6.5 Some built-in functions can't AOP
https://github.com/naver/pinpoint-c-agent/issues/102

