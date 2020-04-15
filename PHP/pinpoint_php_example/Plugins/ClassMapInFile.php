<?php

namespace Plugins;
use pinpoint\Common\AopClassMap;

class ClassMapInFile extends AopClassMap
{
    private $mustHoldClasses = ['app\AccessRemote'=>null,'app\Proxied_AccessRemote'=>null,'Plugins\PerRequestPlugins'=>null];
    public function __construct()
    {
        parent::__construct();
    }

    public  function findFile($classFullName)
    {
        $file = parent::findFile($classFullName);
        if($file){
            if(!PerRequestPlugins::instance()->traceLimit())
            {
                return $file;
            }elseif (array_key_exists($classFullName ,$this->mustHoldClasses) == 1){
                return $file;
            }else{
                echo "reject $$classFullName <br>";
                return null;
            }
        }
        return $file;

//        if(!PerRequestPlugins::instance()->traceLimit())
//        {
//            return parent::findFile($classFullName);
//        }
//
//        if (array_key_exists($classFullName ,$this->mustHoldClasses) == 1)
//        {
//            if(PerRequestPlugins::instance()->traceLimit() && )
//
//            return parent::findFile($classFullName);
//        }else {
//            echo "reject".$classFullName.'<br>';
//            return null;
//        }
    }

}
require __DIR__ . '/PerRequestPlugins.php';