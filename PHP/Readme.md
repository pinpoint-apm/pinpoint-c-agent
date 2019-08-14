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
----|-----|----|-----|---
Laravel|✔|✔
ThinkPHP|✔|✔
EasySwoole|✘|✘

> Note

1. We are trying to support PHP asynchrnous Framework.Such as Workerman and EasySwoole
2. If your php application not support composer(such as woredpress,phpwind etc),the only choise is v0.1.x.

#### Stability

`v0.2.x > v0.1.x`


#### Maintainability(Dynamically)


Functional|v0.1.x|v0.2.x
----|-----|----
1.Update plugins(CRUD)|✘|✔
2.Update pinpoint collector information|✘|✔
3.Update pinpiont_php.so(pinpoint.so)|✘|✘

#### Performance Loss

> Sample Rate： 50% (50% functions are watched)

Amount|None pinpont-agent|v0.1.x|v0.2.x
----|-----|-----|----
20 functions|MTT=2.98ms|MTT=2.63ms|MTT=4.69ms
100 functions|MTT=3.01ms|MTT=6.31ms|MTT=6.21ms
200 functions|MTT=2.89ms|MTT=18.03ms|MTT=7.54ms

> By the way,pinpoint is an APM system, not a performance enhancement system. Loss can't avoiding from now on.


### Contributors's words

As composer is widedly used,v0.2.x is our long-time support version,
v0.1.x could be obsoleted in future.
While, we could fix some fatal error into v0.1.x.