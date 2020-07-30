
步骤

1. 在 public/index.php中启用pinpoint header

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

2. 在composer.json中包含pinpoint-agent驱动程序和插件

2.1 复制`Plugins` 到您的根目录，并启用pst-4自动加载

```json
  "autoload-dev": {
        "psr-4": {
             ...
            "Plugins\\": "Plugins/"
        }
    }
```

2.2 在您的`require`中启用`pinpoint-php-aop`

```json
    "require": {
        ...
        "naver/pinpoint-php-aop": "v1.0.1",
        ...
        }
```

注意：如果php < 7，则使用v0.3+