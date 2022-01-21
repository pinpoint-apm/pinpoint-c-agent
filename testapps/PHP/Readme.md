## How to start testapp
1. Install Composer. [How to Use Composer?](https://getcomposer.org/doc/00-intro.md)
2. Execute `composer install` or `composer update`.
3. Import pinpoint entry header into thinkphp/start.php as below
    ## start.php
    ```
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
4. Start testapp by executing `php -S ip:port router.php` under `public` directory.