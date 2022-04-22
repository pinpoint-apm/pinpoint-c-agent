## How to Use in ThinkPHP5.0x

### Limitation

| -                  | version | checked |
| ------------------ | ------- | ------- |
| PHP                | 7+      | ✔      |
| laravel/framework | ^8.65   | ✔      |

### Steps

1. Import pinpoint entry header into `public/index.php` as below

```php
## index.php

<?php

.....
require __DIR__.'/../vendor/autoload.php'; // <------- below this line ✨

###################################################################
define('AOP_CACHE_DIR', __DIR__ . '/../Cache/');
define('PLUGINS_DIR', __DIR__ . '/../Plugins/');
define('APPLICATION_NAME','app-thinkphp'); // your application name
define('APPLICATION_ID','app-thinkphp');  // your application id
define('PINPOINT_USE_CACHE','YES');
require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
###################################################################

```

2. require `pinpoint-php-aop` into your composer.json

```json
    "require": {
        "php": "^7.3|^8.0",
        "fruitcake/laravel-cors": "^2.0",
        "guzzlehttp/guzzle": "^7.0.1",
        "laravel/framework": "^8.65",
        "laravel/tinker": "^2.5",
        "tymon/jwt-auth": "^1.0",
        "pinpoint-apm/pinpoint-php-aop": "dev-master" <---- require pinpoint-apm
    },
```

3. composer install #( composer update)
4. copy [setting.ini](./setting.ini) into your `PLUGINS_DIR` folder.
