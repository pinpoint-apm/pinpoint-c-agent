<?php
namespace Plugins;
use Plugins\Candy;

class FooTestPlugin extends Candy
{
    ///@hook:app\Foo::foo_p1
    public function onBefore(){
        echo " call onBefore ".__METHOD__."\n";
    }

    public function onEnd(&$ret){
        throw new \Exception("shouldn't call");
    }

    public function onException($e){
        throw new \Exception("shouldn't call");
    }
}