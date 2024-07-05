## Support Plan
What we have supported and what will support: [support plan](SupportPlan.md)

## Getting Started

### Requirement

Dependency|Version| More
---|----|----
PHP| php `7+`|
GCC| GCC `4.7+`| C++11 
*inux|| `windows is on the way`
pinpoint| `2.0+`|
composer| | class can be automatic pinpoint-cut
collector-agent| [installed ?](../collector-agent/readme.md)

### Installation

#### Steps
1. Install pinpoint_php

   curl -sL https://github.com/***pinpoint-apm***/pinpoint-c-agent/releases/download/**v0.1.11**/install_pinpoint_php.sh | sh

2. Check your pinpoint_php.ini is right
         
     ```ini
     extension=pinpoint_php.so
     ; Collector-agent's TCP address, ip,port:Collector-Agent's ip,port, please ensure it consistent with the `PP_ADDRESS` of `Collector-Agent` in step2(Build Collector-Agent).
     pinpoint_php.CollectorHost=Tcp:ip:port
     pinpoint_php.SendSpanTimeOutMs=0 # 0 is recommanded
     ; request should be captured duing 1 second. < 0 means no limited
     pinpoint_php.TraceLimit=-1 
     ; DEBUG the agent, PHP's log_error should turn on too.
     ;pinpoint_php.DebugReport=true
     ;error_reporting = E_ALL
     ;log_errors = On
     ;error_log = /tmp/php_fpm_error.log
     ```
     
3. Install pinpoint-php-aop by composer

     composer require -w pinpoint-apm/pinpoint-php-aop

4. Add entry into your `index.php`

```php
// enable `vendor/autoload.php` if need
// require_once __DIR__ . '/vendor/autoload.php';
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.run');  // your application id
define('PP_REQ_PLUGINS', Pinpoint\Plugins\DefaultRequestPlugin::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
```

>We have prepared some examples for you, try [playground](https://github.com/pinpoint-apm/pinpoint-c-agent/tree/dev/testapps#playground).


## Feedback & Question

### 1. How to regenerate all AOP files?

     Delete /tmp/.cache/.__class_index.php

### 2. Why does not support automatically update AOP files?

We can DO but prefer not to DO! Because we have not yet found an efficient way to implement this and monitoring these files status every time is a bad idea.

### 3. How much performance does it lose when using?

After full test, the AOP code could spend at least 1ms in our env(E5-2660 v4 @ 2.00GHz). While, the more function/method you monitor, the more you take.

#### Performance Test Result

     On the way

### For the developer

[Plugins Tutorial ☚](../../plugins/PHP/Readme.md)

[PHP user manual ☚](./User%20Manual.md)

### Known Issues

#### 1. Exception or error message with a wrong file path.

     https://github.com/pinpoint-apm/pinpoint-c-agent/issues/99

#### 2. If I do not use composer/autoloader, can I use this version?

     we supported none framework(yii,tp,...) project,even a simple php script, try example in testapps/php_wordpress,SimplePHP

