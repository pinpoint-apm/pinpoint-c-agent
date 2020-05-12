## Getting Started

### Requirement

Dependency|Version
---|----
PHP| php 7+
GCC| GCC 5+
*inux|
Python | Python 3
pinpoint| 1.8.0-RC1
**Composer**| 

### Installation

#### Steps
1. git clone https://github.com/naver/pinpoint-c-agent.git
   
> Build pinpoint-php-module

2. Goto PHP/pinpoint_php_ext
   1. phpize        
   2. ./configure
   3. make 
   4. make test # we commanded test the module is RIGHT
   5. make install 
   6. Activation pinpoint module
   
       >  php.ini 
        ```ini
        extension=pinpoint_php.so
        # must be unix:(unix sock address)
        pinpoint_php.CollectorHost=unix:/tmp/collector-agent.sock
        pinpoint_php.SendSpanTimeOutMs=0 # 0 is recommanded
        # request should be captured duing 1 second. < 0 means no limited
        pinpoint_php.TraceLimit=-1 
        ```
> Build Collect-agent
3. Goto collector-agent
   1. python3 -m venv env
   2. source env/bin/activate
   3. pip3 install -r requirements.txt
   4. Change configuration file in conf/collector.conf

        ```ini
        [Collector]
        # collectorhost and specific port
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
        LOG_DIR=/your log dir/
        [Agent]
        # the same as below "pinpoint_php.CollectorHost"
        Address=/tmp/collector-agent.sock
        ```
    5. export COLLECTOR_CONFIG=/full path of collector.conf/
    6. run  collector-agent
        ```
        $ ./init_python_env.sh
        $ python run.py
        ```
   
> Try the aop example
4. Goto PHP/pinpoint_php_example
   1. Download composer.phar. [How To Use Composer?](https://getcomposer.org/doc/00-intro.md)
   2. php composer.phar install
   3. Set $PWD/app/ as your web site root path



### TODO


## Compatibility

Pinpoint Version | PHP| Python|GCC
---------------- | ----- | -----|---
1.8.0-RC1 | PHP 7+ | Python3|GCC 5+

## Changes

- Fully support ZTS
- More Stability 
- Higher performance 
- Easy to Use and Maintain
- Support GRPC

## F & Q

### 1. How to regenerate all AOP files?

Delete *__class_index_table* in Cache/

### 2. Why not support automaticaly update AOP files?

We can DO but perfer not DO! Not found a good efficiency way to implement this.
Watching the files status every time is a bad deal.

### 3. How much performance loss when I use?

After full test, the AOP code could consume at least 1ms in our env(E5-2660 v4 @ 2.00GHz). While, the more function/method you watching, the more you taken. 

[Test Result ☚](PHP/Readme.md#performance-loss-under-stress-test)

### For the developer

[Plugins Tutorial ☚](PHP/pinpoint_php_example/Readme.md)

### Known Issues

#### 1. Exception or Error Message With A Wrong File Path.

https://github.com/naver/pinpoint-c-agent/issues/99

#### 2. If I Do Not Use Composer/autoloader,Can I Use this Version ?

Sorry, `php-simple-aop` not supported wrapping the user class (or internal class) without Composer/autoloader. By the way [Composer](https://getcomposer.org/) is GOOD. O(∩_∩)O

https://github.com/naver/pinpoint-c-agent/issues/103

#### 3. Why not support PHP5.x?

https://www.php.net/supported-versions.php

#### 4. Gernerator function not support.

https://github.com/naver/pinpoint-c-agent/issues/100

#### 5. Some built-in functions cant AOP
https://github.com/naver/pinpoint-c-agent/issues/102


## The Difference Between v0.1.x And v0.2.x

##  v0.1.x VS v0.2.x

> Why we refactor the pinpoint_c_agent (v0.1.x)

1. Hard to support all php versions.
2. Slow, v0.1.x version hooks all functions.
3. Not support ZTS.

### Data route map

#### v0.1.x

> AOP base on zend_excuted_hook

![How does it work](../images/principle_v0.1.x.png)
#### v0.2.x

> AOP base on classloader (like java-classloaders)

![How does it work](../images/principle_v0.2.x.png)

#### PHP Version Compatibility

Agent version|PHP5.5.x|PHP5.6.x|PHP7.x |php-zts
----|-----|----|-----|---
v0.1.x|✔|✔|✔|✘
v0.2.x|✘|✘|✔|✔

#### PHP Framework

Framework|v0.1.x|v0.2.x
----|-----|----|
Laravel|✔|✔
ThinkPHP|✔|✔
EasySwoole|✘|✘

> Note

1. We are trying to support PHP asynchrnous Framework.Such as Workerman and EasySwoole
2. If your php application not support composer(such as woredpress,phpwind etc),the only choise is v0.1.x.

#### Stability

`v0.2.x > v0.1.x`


#### Maintainability(Dynamically)

```
✔: Not block user's application.
✘: Block user's application: php-fpm/apache should be restart
```

Functional|v0.1.x|v0.2.x
----|-----|----
1.Update plugins(CRUD) |✘|✔ [How to ?](https://github.com/eeliu/php_simple_aop#how-to-reload-all-plugins)
2.Update pinpoint collector|✘|✔
3.Update pinpiont_php.so(pinpoint.so)|✘|✘



#### Performance Loss Under Stress Test

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

> By the way,pinpoint is an APM system, not a performance enhancement system. Loss can't avoiding from now on.


### Contributors's words

As composer is widedly used, v0.2.x is our long-time support version,
v0.1.x could be obsoleted in future.
While, we could fix some fatal error into v0.1.x.