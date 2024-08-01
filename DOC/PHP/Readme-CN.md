## 安装参照

### 依赖组件

| Dependency          | Version                                     | More                                |
| ------------------- | ------------------------------------------- | ----------------------------------- |
| PHP                 | php `7+`                                    |
| *inux/windows/macOs |                                             | windows is released                 |
| pinpoint            | `2.0+`                                      |
| composer            |                                             | class can be automatic pinpoint-cut |
| collector-agent     | [installed ?](../collector-agent/readme.md) |

### 安装步骤

1. 安装 `ext-pinpoint_php` 模块

     `pecl install pinpoint_php`
     or 
     `curl -sL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/latest/install_pinpoint_php.sh | sh`

     > On windows: download .dll from  https://pecl.php.net/package/pinpoint_php  or https://github.com/pinpoint-apm/pinpoint-c-agent/releases/latest


2. 检查下你的配置文件 pinpoint_php.ini
         
     ```ini
     extension=pinpoint_php.so
     ; Collector-agent's TCP address, ip,port:Collector-Agent's ip,port, please ensure it consistent with the `PP_ADDRESS` of `Collector-Agent` in step2(Build Collector-Agent).
     pinpoint_php.CollectorHost=Tcp:ip:port
     pinpoint_php.SendSpanTimeOutMs=0 # 0 is recommended
     ; request should be captured during 1 second. < 0 means no limited
     pinpoint_php.TraceLimit=-1 
     ; DEBUG the agent, PHP's log_error should turn on too.
     ;pinpoint_php.DebugReport=true
     ;error_reporting = E_ALL
     ;log_errors = On
     ;error_log = /tmp/php_fpm_error.log
     ```
     
3. 安装`pinpoint-php-aop` 依赖，用来管理切面代码

     composer require -w pinpoint-apm/pinpoint-php-aop

4. 在入口文件(eg: index.php)中引入 pinpoint-apm

```php
// enable `vendor/autoload.php` if need
// require_once __DIR__ . '/vendor/autoload.php';
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.run');  // your application id
define('PP_REQ_PLUGINS', Pinpoint\Plugins\DefaultRequestPlugin::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
```

> 我们准备了一些例子给您, 里面包含了写PHP常用项目的安装脚本
> [playground](https://github.com/pinpoint-apm/pinpoint-c-agent/tree/dev/testapps#playground).

如果还有疑问，给我们提个issue吧 🙋‍♂️


