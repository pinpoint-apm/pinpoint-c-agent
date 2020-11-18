## Steps

* copy pinpoint header into index.php

```php
define('AOP_CACHE_DIR', __DIR__ . '/../Cache/');
define('PLUGINS_DIR', __DIR__ . '/../Plugins/');
define('APPLICATION_NAME','app-cakephp');
define('APPLICATION_ID','app-cakephp');
define('PINPOINT_USE_CACHE','YES');
define('PP_REQ_PLUGINS', '\Plugins\PerRequestPlugins');
require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';

```

* copy setting.ini into `Plugins`