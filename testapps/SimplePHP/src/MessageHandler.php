<?php
namespace SimplePHP;

class MessageHandler
{
    public function handle_message_in_kafka($message)
    {
        echo "$message->topic_name $message->partition\n";
    }

}
