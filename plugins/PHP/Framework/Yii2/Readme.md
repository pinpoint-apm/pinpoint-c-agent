# How to Use in Yii2

## Limitation

-|version|checked
---|----|---
PHP|7+|✔


## Steps


1. Import pinpoint entry header into `web/index.php` as below


```php
## index.php

<?php

// comment out the following two lines when deployed to production
defined('YII_DEBUG') or define('YII_DEBUG', true);
defined('YII_ENV') or define('YII_ENV', 'dev');

require __DIR__ . '/../vendor/autoload.php';
require __DIR__ . '/../vendor/yiisoft/yii2/Yii.php';


###################################################################
define('APPLICATION_NAME','app-yii2'); // your application name
define('APPLICATION_ID','app-yii2');  // your application id
define('PP_REQ_PLUGINS','\Pinpoint\Plugins\Yii2PerRequestPlugins');
require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
###################################################################


$config = require __DIR__ . '/../config/web.php';

(new yii\web\Application($config))->run();


```

2. require `pinpoint-php-aop` into your composer.json


``` json
    "require": {
        ...
        "pinpoint-apm/pinpoint-php-aop": "dev-master"  <-- import pinpoint-php-aop
    },
```

3. composer install #( composer update)
4. copy [setting.ini](./setting.ini) into your `PLUGINS_DIR` folder.

>ps Tell pinpoint-php-aop your framework is yii2
