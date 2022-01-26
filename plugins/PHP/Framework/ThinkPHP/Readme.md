## How to Use in ThinkPHP5.0x

### Limitation

| -                  | version | checked |
| ------------------ | ------- | ------- |
| PHP                | 7+      | ✔      |
| topthink/framework | 6.*   | ✔      |

### Steps

1. Import pinpoint entry header into `thinkphp/start.php` as below

```php
## start.php

<?php

namespace think;

// ThinkPHP 引导文件
// 1. 加载基础文件
require __DIR__ . '/base.php';

###################################################################
define('AOP_CACHE_DIR', __DIR__ . '/../Cache/');
define('PLUGINS_DIR', __DIR__ . '/../Plugins/');
define('APPLICATION_NAME','app-thinkphp'); // your application name
define('APPLICATION_ID','app-thinkphp');  // your application id
define('PINPOINT_USE_CACHE','YES');
require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
###################################################################
// 2. 执行应用
App::run()->send();

```

2. require `pinpoint-php-aop` into your composer.json

```json
    "require": {
        "php": ">=5.4.0",
        "topthink/framework": "5.0.*",
        "pinpoint-apm/pinpoint-php-aop": "dev-master"  <-- import pinpoint-php-aop
    },
```

3. composer install #( composer update)
4. copy [setting.ini](./setting.ini) into your `PLUGINS_DIR` folder.

> ps Tell pinpoint-php-aop your framework is Thinkph>p5.0.x !
