## How to use ?

### Steps

1. Include php_simple_aop into composer.json
 
```
   "require": {
        "eeliu/php_simple_aop": "dev-user-classmap"
    }
```

2. Copy plugins into web root path and enable autoload

```
    "autoload": {
        "psr-4": {
            
             "Plugins\\": "Plugins/"
        }
    }
```

3. Insert pinpoint header into site.php

``` php
#####################################################################################
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR',__DIR__.'/Plugins/');
define('APPLICATION_NAME','Flarum');
define('APPLICATION_ID','Flarum');
// Support party loader
// define('USER_DEFINED_CLASS_MAP_IMPLEMENT','\Plugins\ClassMapInFile.php');
require_once __DIR__. '/vendor/eeliu/php_simple_aop/auto_pinpointed.php';

#####################################################################################
```

## Output

### CallStack
![CallStack](images/Flarum_callstack.png)

### ServerMap
![CallStack](images/Flarum_readme.png)