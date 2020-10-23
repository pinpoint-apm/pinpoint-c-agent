## Steps

### 1. copy setting.ini

### 2. enable pinpoint entry into easyswoole

```php
#################################################
define('AOP_CACHE_DIR',__DIR__.'/Cache/');
define('PLUGINS_DIR', __DIR__ . '/Plugins/');
//define('USER_DEFINED_CLASS_MAP_IMPLEMENT',"Plugins\Framework\app\ClassMapInFile");
define('PER_REQ_CLASS_NAME','Plugins\Framework\Swoole\Http\Server\PerReqPlugin');
define('APPLICATION_NAME','APP-3');
define('APPLICATION_ID','app-3');
define("PINPOINT_USE_CACHE",'NO');
require_once __DIR__. '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
#################################################
```

[ read the  example please ](easyswoole_example.txt)

