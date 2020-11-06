<?php
/*
 * User: eeliu
 * Date: 11/6/20
 * Time: 4:55 PM
 */

namespace Plugins\Sys\RdKafka;


class ProfilerNewTopic
{
    protected $_instance;
    public $blocker_list;
    public $topic;
    public function __construct($instance,$blocker_list,$topic)
    {
        $this->_instance = &$instance;
        $this->blocker_list = $blocker_list;
        $this->topic = $topic;
    }

    public function __call($name, $arguments)
    {
        return call_user_func_array([$this->_instance,$name],$arguments);
    }

    public function produce($partition,$msgflags,$payload,$key=null)
    {
        $param = \pinpoint_get_func_ref_args();

        $plugin = new ProducePlugin("Topic::produce",$this,$param);
        try{
            $plugin->onBefore();
            $ret =  call_user_func_array([$this->_instance,'produce'],$param);
            $plugin->onEnd($ret);
            return $ret;
        }catch (\Exception $e)
        {
            $plugin->onException($e);
        }
    }
}