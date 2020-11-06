<?php
/*
 * User: eeliu
 * Date: 11/6/20
 * Time: 4:54 PM
 */

namespace Plugins\Sys\RdKafka;


use Plugins\Common\Candy;

class ProducePlugin extends Candy
{

    function onBefore()
    {
        $blocker_list = $this->who->blocker_list;
        $topic = $this->who->topic;
        pinpoint_add_clue(PP_SERVER_TYPE,PP_KAFKA);
        pinpoint_add_clues(PP_KAFKA_TOPIC,$topic);
        pinpoint_add_clue(PP_DESTINATION,$blocker_list);
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }
}