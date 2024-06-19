## 支持计划
我们已经支持以及将要支持的计划详情: [支持计划](SupportPlan.md)

## 入门指南

### 要求

依赖|版本|附加
---|----|----
PHP| php `7+`|
GO | | 
GCC| GCC `4.7+`| C++11 
cmake| cmake `3.2+`|
*inux|| `windows is on the way`
pinpoint| `2.0+`|
composer| | class can be automatic pinpoint-cut
collector-agent| [installed ?](../collector-agent/readme.md)

### 安装

#### 步骤
1. 执行命令：git clone https://github.com/pinpoint-apm/pinpoint-c-agent.git

2. 安装 pinpoint-php-module， 在pinpoint-c-agent安装包的根目录下，执行以下命令：
   1. phpize        
   2. ./configure
   3. make 
   4. make test TESTS=src/PHP/tests/ （我们建议您执行此命令，测试该模块是否正确。PHP5请执行```make test TESTS=src/PHP/tests5/```）
   5. make install 
   6. 激活 pinpoint module，请将以下内容添加到你的 ```php.ini``` 配置文件中
   
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
        
3. 在您的项目中使用Pinpoint PHP-Agent，请执行以下步骤：

   我们假设您已经安装composer，并了解怎么使用composer[如何使用 Composer?](https://getcomposer.org/doc/00-intro.md)

   1. 安装 ```pinpoint-apm/pinpoint-php-aop```, 在```composer.josn```的```require```中添加```"pinpoint-apm/pinpoint-php-aop": "v2.0.1"```:
        ```
        "require": {
            ...
            "pinpoint-apm/pinpoint-php-aop": "v2.0.1"
        }
        ```
   2. 在项目的入口文件中添加下面的常量：
        ```
        #################################################
        define('APPLICATION_NAME','APP-2');
        define('APPLICATION_ID','app-2');
        define('PP_REQ_PLUGINS', '\Plugins\PerRequestPlugins');
        require_once __DIR__. path to 'vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
        #################################################
        ```
        1. ```APPLICATION_NAME```: 应用名称.
        2. ```APPLICATION_ID```: Agent ID.
        3. ```PP_REQ_PLUGINS```: ```PerRequestPlugins```的路径(```PerRequestPlugins```是基本的请求拦截器, 不同的PHP框架的拦截器不同，我们已经为您准备了一些框架的```PerRequestPlugins```，[请到这里获取](../../plugins/PHP/Plugins/Framework)，（例如:[swoole's PerRequestPlugins](../../plugins/PHP/Plugins/Framework/Swoole/Http/PerReqPlugin.php)）。欢迎PR其他框架哟~
        7. ```require_once __DIR__. path to 'vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';```: 导入pinpoint的auto_pinpointed.php。**请在```require_once __DIR__."/../vendor/autoload.php";```之后添加，这很重要**

   3. 拷贝[Plugins](../../plugins/PHP/Plugins)到您的项目根路径下，并在```composer.josn```中自动加载```Plugins```. 
         Frameworks:
            [ThinkPHP5](../../plugins/PHP/Framework/ThinkPHP5)
            [Yii2](../../plugins/PHP/Framework/Yii2)
            [laravel](../../plugins/PHP/Framework/laravel)
    
      另外，我们还准备了一些例子以供参考：[testapps](../../testapps/PHP)。


##  变化

- 完全支持 ZTS
- 更加稳定
- 性能更高
- 易于使用和维护
- 支持 GRPC

## F & Q

### 1. 如何重新生成所有AOP文件？

 在Cache/中删除 __class_index_table

### 2. 为什么不支持自动更新AOP文件?

我们可以支持但不建议这样做！因为我们还没有找到一种有效的方法来实现这一点，每次监测这些文件的状态结果都很糟糕。

### 3.  在使用时损失了多少性能？

在完全测试之后，AOP代码可以在我们的环境(E5-2660 v4 @ 2.00GHz)中至少消耗1ms。但监测的功能/方法越多，损耗的性能也就越多。

#### 性能测试结果

##### 3.1 [ 测试结果 ☚](./detail_versions-CN.md#压力测试下的性能损失)

##### 3.2 [Flarum的测试结果 ☚](./User%20Manual-CN.md#1.1-性能测试结果)

### 对于开发人员

[插件编写指南  ☚](../../plugins/PHP/Readme.md)

[PHP用户手册  ☚](./User%20Manual-CN.md)

### 已知问题

#### 1. 带有错误文件路径的异常或错误消息。

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/99

#### 2. 如果不使用composer/autoloader，我可以使用这个版本吗？

抱歉，`pinpoint-php-aop` 不支持打包没有composer/autoloader的用户类(或内部类)。顺便说一下，composer挺不错的O(∩_∩)O

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/103

#### 3. ~为什么不支持PHP5.x? ~此问题已解决！

https://www.php.net/supported-versions.php

#### 4. ~不支持generator函数。~此问题已解决!

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/100

#### 5. 一些内置函数不支持AOP。
https://github.com/pinpoint-apm/pinpoint-c-agent/issues/102
