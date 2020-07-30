## 支持计划
我们已经支持以及将要支持的计划详情: [支持计划](SupportPlan.md)

## 入门指南

### 要求

依赖|版本
---|----
PHP| php `7+` ,`5+`
GCC| GCC `4.7+`
*inux|
Python | `Python 3`
pinpoint| `1.8.0+`, `2.0+`
**Composer**| 

### 安装

#### 步骤
1. 执行此命令：git clone https://github.com/naver/pinpoint-c-agent.git
   
    > 搭建 pinpoint-php-module

2. 在您的pinpoint-c-agent安装包的根目录下，执行以下命令：
   1. phpize        
   2. ./configure
   3. make 
   4. make test TESTS=src/PHP/tests/ # 我们建议您执行此命令，测试该模块是否正确
   5. make install 
   6. Activation pinpoint module # 将以下内容添加到 php/lib/php.ini 的配置文件中
   
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
     > 搭建 Collector Agent
3. [Collector Agent 安装步骤 ☚](../CollectorAgent/Readme-CN.md)
   
     > 试试 aop example
4. 在 Example/PHP 目录下（如果使用的是php5，请到 Example/PHP5 目录下），执行以下步骤：
   1. 下载 composer.phar. [如何使用 Composer?](https://getcomposer.org/doc/00-intro.md)
   2. 安装 php composer.phar
   3. 将$PWD/app/设置为web根路径



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

[插件编写指南  ☚](../../Example/PHP/Readme-CN.md)

[PHP用户手册  ☚](./User%20Manual-CN.md)

### 已知问题

#### 1. 带有错误文件路径的异常或错误消息。

https://github.com/naver/pinpoint-c-agent/issues/99

#### 2. 如果不使用composer/autoloader，我可以使用这个版本吗？

抱歉，`pinpoint-php-aop` 不支持打包没有composer/autoloader的用户类(或内部类)。顺便说一下，composer挺不错的O(∩_∩)O

https://github.com/naver/pinpoint-c-agent/issues/103

#### 3. ~为什么不支持PHP5.x? ~此问题已解决！

https://www.php.net/supported-versions.php

#### 4. ~不支持generator函数。~此问题已解决!

https://github.com/naver/pinpoint-c-agent/issues/100

#### 5. 一些内置函数不支持AOP。
https://github.com/naver/pinpoint-c-agent/issues/102


> [v0.1.x 和 v0.2.+ 的区别](./detail_versions-CN.md)