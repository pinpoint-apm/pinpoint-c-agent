## Steps for Writing Plugins For Swoole framework

### 1. Add a pre request plugin on the entry.

```  php 
///@hook:example\swoole\HandleRequest::onReceive
class TcpServerPerRequestPlugin extends Candy
{
 public function onBefore(){}
 public function onEnd(&$ret){}
}
```

### 2. Add common plugins on others. (the same as pinpoint_php_example/Plugins)

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