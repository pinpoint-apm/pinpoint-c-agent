## 지원 계획
지원 버전 또한 향후 확장 지원 계획 상세: [지원 계획](SupportPlan.md)

## 시작 가이드 

### 요구

디펜던시 | 버전 | 부가
---|----|----
PHP| php `7+`|
GO | | 
GCC| GCC `4.7+`| C++11 
cmake| cmake `3.2+`|
*inux|| `windows is on the way`
pinpoint| `2.0+`|
composer| | class can be automatic pinpoint-cut
collector-agent| [installed ?](../collector-agent/readme.md)

### 설치

#### 스텝
1. 명령 실행：git clone https://github.com/pinpoint-apm/pinpoint-c-agent.git

2. pinpoint-php-module 설치하여 pinpoint-c-agent 페키지 루트 디렉토리에서 이하 명령 실행：
   1. phpize        
   2. ./configure
   3. make 
   4. make test TESTS=src/PHP/tests/ （해당 모듈의 정확성을 테스트하기 위하여 이 명령의 실행을 권장 드립니다. PHP5의 경우 ```make test TESTS=src/PHP/tests5/``` 실행하시기 바랍니다.）
   5. make install 
   6. pinpoint module 활성화시켜 아래 내용을  ```php.ini```  컨피그 파일에 추가해 주십시오.

       >  php.ini 
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
        
3. 프로젝트에서 Pinpoint PHP-Agent를 이용하여 아래 스탭을 진행하십시오. 

   이미 composer를 설치하신 것을 가정하여 어떻게 composer를 사용하여야 하는지에 대해 알아봅시다. [Composer 어떻게 사용하여야 하는가?](https://getcomposer.org/doc/00-intro.md)

   1. ```pinpoint-apm/pinpoint-php-aop``` 설치하여 ```composer.josn```의 ```require```에 ```"pinpoint-apm/pinpoint-php-aop": "v2.0.1"``` 추가:
        ```
        "require": {
            ...
            "pinpoint-apm/pinpoint-php-aop": "v2.0.1"
        }
        ```
   2. 프로젝트의 엔트리 파일에 아래 상수 추가：
        ```
        #################################################
        define('APPLICATION_NAME','APP-2');
        define('APPLICATION_ID','app-2');
        define('AOP_CACHE_DIR',__DIR__.'./Cache/');
        define('PLUGINS_DIR',__DIR__.'./Plugins/');
        define('PINPOINT_USE_CACHE','YES');
        define('PP_REQ_PLUGINS', '\Plugins\PerRequestPlugins');
        #define('USER_DEFINED_CLASS_MAP_IMPLEMENT',"\Plugins\Framework\app\ClassMapInFile");
        require_once __DIR__. path to 'vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';
        #################################################
        ```
        1. ```APPLICATION_NAME```: 애플리케이션 이름.
        2. ```APPLICATION_ID```: Agent ID.
        3. ```AOP_CACHE_DIR```: ```Cache```폴더 위치 저장.
        4. ```PLUGINS_DIR```: ```Plugins```폴더 경로.
        5. ```PINPOINT_USE_CACHE```: 'YES'-요청이 있을 때 자동 업데이트 하지 않음  ```Cache```; 'No'-요청이 있을 때 자동 업데이트 진행  ```Cache```(```Cache``` 수동 업데이트도 가능하여 ```Cache``` 직접 삭제하시면 됨). 성능을 고려하시면 'YES'로 설정하는 것을 권장 드립니다. Plugins를 수정하시는 경우 ```Cache```를 업데이트하셔야 반영이 될 수 있습니다.
        6. ```PP_REQ_PLUGINS```: ```PerRequestPlugins```의 경로( ```PerRequestPlugins```는 기본적인 요청 차단기입니다. 다른 PHP 프레임워크에 따라 차단기가 다를 수가 있습니다. 일부 프레임워크의 ```PerRequestPlugins```는 이미 준비해 놓았으니 [여기에 들어가서 받으십시오](../../plugins/PHP/Plugins/Framework)（예제 :[swoole's PerRequestPlugins](../../plugins/PHP/Plugins/Framework/Swoole/Http/PerReqPlugin.php)）. 물론 다른 프레임워크에 관한 PR도 환영합니다~
        7. ```require_once __DIR__. path to 'vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';```: pinpoint의 auto_pinpointed.php를 임포트합니다.** ```require_once __DIR__."/../vendor/autoload.php";```뒤에 추가하는 것은 유의하시기 바랍니다. **

   3. [Plugins](../../plugins/PHP/Plugins)를 프로젝트 루트 경로에 복제하여 ```composer.josn```에서 ```Plugins```를 자동 적재합니다. 
         Frameworks:
            [ThinkPHP5](../../plugins/PHP/Framework/ThinkPHP5)
            [Yii2](../../plugins/PHP/Framework/Yii2)
            [laravel](../../plugins/PHP/Framework/laravel)
    
      이 외에, 다른 예제도 제공되어 있으니 필요하시면 참고하시기 바랍니다：[testapps](../../testapps/PHP)。


##  변경

- ZTS 지원
- 안정성 향상 
- 성능 향상 
- 사용성과 운영성 강화
- GRPC 지원

## F & Q

### 1. 모든 AOP 파일은 어떻게 재생성할 수 있는가？

Cache/에서 __class_index_table를 삭제하면 됩니다.

### 2. 왜 AOP 파일의 자동 업데이트는 지원되지 않는가?

저희가 지원할 수 있으나 권장 드리지 않는 것입니다. 왜냐하면 아직 구현하기에 유효한 방법을 찾아내지 못했기 때문입니다. 관련 파일의 상태를 모니터링할 때마다 좋은 결과로 나오지 않았습니다.  

### 3.  이용 시 얼마나의 성능을 소모할 수 있는 것인가？

테스트를 거쳐 AOP 코드는 저희의 환경(E5-2660 v4 @ 2.00GHz)에서 최소 1ms를 소모하는 것으로 확인되었습니다. 그러나 모니터링의 기능 또한 방법이 많을 수록 소모된 성능도 역시 많아지게 될 것입니다.

#### 프로파일링 결과 

##### 3.1 [ 테스트 결과 ☚](./detail_versions-CN.md# 스트레스 테스트에서의 성능 소모)

##### 3.2 [Flarum의 테스트 결과 ☚](./User%20Manual-CN.md#1.1-프로파일링 결과)

### 개발자 상관 

[플러그인 작성 가이드  ☚](../../plugins/PHP/Readme.md)

[PHP유저 가이드  ☚](./User%20Manual-CN.md)

### 노운 이슈 

#### 1. 잘못된 파일 경로를 가지고 있는 이상 또는 에러 메시지 

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/99

#### 2. 만약 composer/autoloader 사용하지 않으면 해당 버전을 사용할 수 있는가? 

너무 죄송한데 `pinpoint-php-aop` 는 composer/autoloader가 없는 커스텀 클래스 또는 내부 클래스를 패키징하는 것을 지원하지 않습니다.  한마디 덧붙이면, composer를 사용해 보시는 것은 추천 드립니다~ O(∩_∩)O

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/103

#### 3. ~왜 PHP5.x를 지원하지 않는가? ~해당 이슈 이미 해결되었음！

https://www.php.net/supported-versions.php

#### 4. ~generator 함수를 지원하지 않습니다。~해당 이슈 이미 해결되었음!

https://github.com/pinpoint-apm/pinpoint-c-agent/issues/100

#### 5. 일부 빌트인 함수는 AOP를 지원하지 않습니다.
https://github.com/pinpoint-apm/pinpoint-c-agent/issues/102
