<?php
/**
 * Created by PhpStorm.
 * User: eeliu
 * Date: 10/10/20
 * Time: 5:49 PM
 */

namespace Plugins\Framework\Swoole\Http;


use Plugins\Common\InsWrapper;

class Response  extends \swoole_http_response
{
    protected $origin;
    public function __construct(&$origin)
    {
        $this->origin = $origin;
    }

    public function status($http_code,$reason=NULL)
    {
        echo "------$http_code----------------";

        return $this->origin->status($http_code,$reason);
    }

//    public function __call($name, $arguments)
//    {
//        return call_user_func_array([$this->origin,$name],$arguments);
//    }
//
//    public static function __callStatic($name, $arguments)
//    {
//       throw new \Exception(";;;;;");
//    }
}