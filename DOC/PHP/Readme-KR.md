## 시작 가이드 

### 요구

| 디펜던시            | 버전                                        | 부가                                |
| ------------------- | ------------------------------------------- | ----------------------------------- |
| PHP                 | php `7+`                                    |
| *inux/windows/macOs |                                             | windows is released                 |
| pinpoint            | `2.0+`                                      |
| composer            |                                             | class can be automatic pinpoint-cut |
| collector-agent     | [installed ?](../collector-agent/readme.md) |

### 설치 스텝

1. `ext-pinpoint_php` 모듈 설치 
    curl -sL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.6.0/install_pinpoint_php.sh | sh

2. pinpoint_php.ini 설정 파일 확인
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

3. 애스펙트 코드 관리용으로 `pinpoint-php-aop` 디펜던시 설치 

     composer require -w pinpoint-apm/pinpoint-php-aop

4. 엔트리 파일(eg: index.php)에서 pinpoint-apm 연동

```php
// enable `vendor/autoload.php` if need
// require_once __DIR__ . '/vendor/autoload.php';
define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.run');  // your application id
define('PP_REQ_PLUGINS', Pinpoint\Plugins\DefaultRequestPlugin::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
```

> PHP로 작성된 일반적인 프로젝트의 설치 스크립트가 포함된 예시를 공유하겠습니다.
> [playground](https://github.com/pinpoint-apm/pinpoint-c-agent/tree/dev/testapps#playground).

추가 문의 사항이 있으시면 언제든지 이슈를 올려주시기 바랍니다. 🙋‍♂️
