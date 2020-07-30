
Steps

1. Enable pinpoint header into public/index.php

``` php
// copy from thinkphp6 public
namespace think;
require __DIR__ . '/../vendor/autoload.php';

########################################################################
define('AOP_CACHE_DIR', __DIR__ . '/../Cache/');
define('PLUGINS_DIR', __DIR__ . '/../Plugins/');
define('APPLICATION_NAME', 'tp6');
define('APPLICATION_ID', 'tp6');
define('PINPOINT_ENV','dev');
// Support party loader
define('USER_DEFINED_CLASS_MAP_IMPLEMENT', '\Plugins\ClassMapInFile');
require_once dirname(__DIR__) . '/vendor/naver/pinpoint-php-aop/auto_pinpointed.php';
########################################################################
// 执行HTTP应用并响应
$http = (new App())->http;

$response = $http->run();
......


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

2.2 Enable `pinpoint-php-aop` into your `require`

```json
    "require": {
        ...
        "naver/pinpoint-php-aop": "v1.0.1",
        ...
        }
```

Note: if php < 7, use `v0.3+`