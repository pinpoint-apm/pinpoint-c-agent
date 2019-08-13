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

#### Stability

`v0.2.x > v0.1.x`


#### Maintainability(Dynamically)


Functional|v0.1.x|v0.2.x
----|-----|----
1.Update plugins(CRUD)|✘|✔
2.Update pinpoint collector info|✘|✔
3.Update pinpiont_php.so(pinpoint.so)|✘|✘
