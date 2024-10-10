<?php

require_once __DIR__ . '/vendor/autoload.php';


class ConsumerPlugins extends \Pinpoint\Plugins\EmptyRequestPlugin
{
    public function joinedClassSet(): array
    {
        $ar = parent::joinedClassSet();
        $classHandler = new \Pinpoint\Common\AspectClassHandle(\SimplePHP\MessageHandler::class);
        $classHandler->addJoinPoint('handle_message_in_kafka', \Pinpoint\Plugins\Common\CommonPlugin::class);
        $ar[] = $classHandler;
        return $ar;
    }
}


define('APPLICATION_NAME', 'cd.dev.test.php'); // your application name
define('APPLICATION_ID', 'cd.dev.test.run');  // your application id
define("PP_ENABLE_EXPERIMENTAL_FEATURE", true);
define('PP_REQ_PLUGINS', ConsumerPlugins::class);
require_once __DIR__ . '/vendor/pinpoint-apm/pinpoint-php-aop/auto_pinpointed.php';


// https://arnaud.le-blanc.net/php-rdkafka-doc/phpdoc/rdkafka.examples-high-level-consumer.html
$conf = new RdKafka\Conf();

$conf->set('group.id', 'myConsumerGroup');

// Initial list of Kafka brokers
$conf->set('metadata.broker.list', 'dev-kafka:9092');
// $conf->set('auto.offset.reset', 'earliest');
$consumer = new RdKafka\KafkaConsumer($conf);

// Subscribe to topic 'test'
$consumer->subscribe(['test']);

while (true) {
    $message = $consumer->consume(120 * 1000);
    switch ($message->err) {
        case RD_KAFKA_RESP_ERR_NO_ERROR:

            $handler = new SimplePHP\MessageHandler();
            $handler->handle_message_in_kafka($message);
            break;
        case RD_KAFKA_RESP_ERR__PARTITION_EOF:
            echo "No more messages; will wait for more\n";
            break;
        case RD_KAFKA_RESP_ERR__TIMED_OUT:
            echo "Timed out\n";
            break;
        default:
            throw new \Exception($message->errstr(), $message->err);
            break;
    }
}
