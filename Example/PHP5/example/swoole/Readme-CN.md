## 为Swoole框架编写插件的步骤

### 1. 在条目上添加一个pre请求插件。

```  php 
///@hook:example\swoole\HandleRequest::onReceive
class TcpServerPerRequestPlugin extends Candy
{
 public function onBefore(){}
 public function onEnd(&$ret){}
}
```

### 2. 在其他插件上添加common plugins (和 pinpoint_php_example/Plugins一样)

``` php
///@hook:example\UserManagerment::checkUser example\UserManagerment::register example\UserManagerment::cacheUser
class CommonPlugin extends Candy
{
    public function onBefore(){
        pinpoint_add_clue("stp",PHP_METHOD);
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    public function onEnd(&$ret){
        pinpoint_add_clues(PHP_RETURN,'xxx');
    }

    public function onException($e){
        pinpoint_add_clue("EXP",$e->getMessage());
    }
}
```