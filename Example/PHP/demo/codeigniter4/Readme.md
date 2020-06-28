
Steps

1. Enable pinpoint header into public/index.php

``` php
// copied from codeigniter4/ public/index.php 
... 
$app = require rtrim($paths->systemDirectory, '/ ') . '/bootstrap.php';
########################################################################
define('AOP_CACHE_DIR', __DIR__ . '/../Cache/');
define('PLUGINS_DIR', __DIR__ . '/../Plugins/');
define('APPLICATION_NAME', 'symfony');
define('APPLICATION_ID', 'symfony');
//define('PINPOINT_ENV','dev');
// Support party loader
define('USER_DEFINED_CLASS_MAP_IMPLEMENT', '\Plugins\ClassMapInFile');
require_once dirname(__DIR__) . '/vendor/eeliu/php_simple_aop/auto_pinpointed.php';
########################################################################

$app->run();


```

2. Include pinpoint-agent driver and plugins into composer.json

2.1 Copy `Plugins` into your root and enable pst-4 autoload

```json
  "autoload-dev": {
        "psr-4": {
             ...
            "Plugins\\": "Plugins/"
        }
    }
```

2.2 Enable `php_simple_aop` into your `require`

```json
    "require": {
        ...
        "eeliu/php_simple_aop": "v0.2.4",
        ...
        }
```

Note if php < 7, use `v0.3.0`