<?php
/*
 * User: eeliu
 * Date: 11/6/20
 * Time: 10:15 AM
 */

namespace app;

use RdKafka;

class TestKafka
{
    private $topic;
    private $rk;
    public function __construct()
    {
        $conf = new RdKafka\Conf();
        $conf->set('log_level', (string) LOG_DEBUG);
        $conf->set('debug', 'all');
        $rk = new RdKafka\Producer($conf);
        $rk->addBrokers("dev-kafka:9092");
        $this->topic = $rk->newTopic("test-pinpoint");
        $this->rk = $rk;
    }

    public function test()
    {
        $this->topic->produce(RD_KAFKA_PARTITION_UA ,RD_KAFKA_MSG_F_BLOCK ,"hello pinpoint");
        $this->topic->produce(RD_KAFKA_PARTITION_UA ,RD_KAFKA_MSG_F_BLOCK ,"hello pinpoint");
        $this->topic->produce(RD_KAFKA_PARTITION_UA ,RD_KAFKA_MSG_F_BLOCK ,"hello pinpoint");

        var_dump($this->topic->getName());
    }

    public function __destruct()
    {
        $this->rk->flush(100);
    }
}