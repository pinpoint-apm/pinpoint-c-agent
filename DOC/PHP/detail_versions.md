# The Difference Between v0.1.x And v0.2.x

##  v0.1.x VS. v0.2+

> Why do we refactor the pinpoint_c_agent (v0.1.x)?

1. Hard to support all php versions.
2. Slow, v0.1.x version hooks all functions.
3. Not support ZTS.

### Data route map

#### v0.1.x

> AOP base on zend_excuted_hook

![How does it work](../../images/principle_v0.1.x.png)
#### v0.2+

> AOP base on classloader (like java-classloaders)

![How does it work](../../images/principle_v0.2.x.png)

#### PHP Version Compatibility

Agent version|PHP5.5.x|PHP5.6.x|PHP7.x |php-zts
----|-----|----|-----|---
v0.1.x|✔|✔|✔|✘
v0.2.x|✔|✔|✔|✔

#### PHP Framework

Framework|v0.1.x|v0.2+
----|-----|----|
Laravel|✔|✔
ThinkPHP|✔|todo
YII|✔|✔
Workerman|✘|✔
EasySwoole|✘|✔

> Note

1. If your php application does not support composer(such as woredpress,phpwind etc), the only choice is v0.1.x.

#### Stability

`v0.2+ > v0.1.x`


#### Maintainability(Dynamically)

```
✔: Not block user's application.
✘: Block user's application: php-fpm/apache should be restart.
```

Operation|v0.1.x|v0.2.x
----|-----|----
1.Update plugins(CRUD) |✘|✔ [How to ?](https://github.com/eeliu/php_simple_aop#how-to-reload-all-plugins)
2.Update pinpoint collector|✘|✔
3.Update pinpiont_php.so(pinpoint.so)|✘|✘

### Contributors' words

As composer is widely used, v0.2+ is our long-time support version, v0.1.x could be obsoleted in the future. But we could fix some fatal error into v0.1.x.



