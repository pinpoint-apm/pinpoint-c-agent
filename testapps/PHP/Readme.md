## How to start testapp
1. Install Composer. [How to Use Composer?](https://getcomposer.org/doc/00-intro.md)
2. Execute `composer install` or `composer update`.
3. Import pinpoint entry header into public/index.php as below
    ## start.php
    ```
    <?php

    ...

    // [ 应用入口文件 ]
    namespace think;

    // 加载基础文件
    require __DIR__ . '/../thinkphp/base.php';

    //###################################################################
    define('AOP_CACHE_DIR', __DIR__ . '/../Cache/');
    define('PLUGINS_DIR', __DIR__ . '/../Plugins/');
    define('APPLICATION_NAME','thinkphp5.1'); // your application name
    define('APPLICATION_ID','thinkphp5.1');  // your application id
    define('PINPOINT_USE_CACHE','YES');
    require_once __DIR__ . '/../vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
    //###################################################################

    // 支持事先使用静态方法设置Request对象和Config对象

    // 执行应用并响应
    Container::get('app')->run()->send();
    ```
4. Start testapp by executing `php think run`.