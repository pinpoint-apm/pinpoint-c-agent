<?php
/*
 * User: eeliu
 * Date: 11/6/20
 * Time: 11:02 AM
 */

namespace Plugins\Sys\RdKafka;

class Producer extends \RdKafka\Producer
{
    public $blocker_list;
    public function addBrokers($blocker_list)
    {
        $this->blocker_list = $blocker_list;
        return parent::addBrokers($blocker_list);
    }

    public  function newTopic($topic,$topic_conf=NULL)
    {
        return new ProfilerNewTopic(parent::newTopic($topic,$topic_conf),$this->blocker_list,$topic);
    }

}