<?php

namespace Plugins;


namespace Plugins;
use Plugins\Candy;

///@hook:MongoDB\Collection::insertOne
///@hook:MongoDB\Collection::findOne
///@hook:MongoDB\Collection::updateMany
///@hook:MongoDB\Collection::deleteMany
class MongoPlugin extends Candy
{
    function onBefore()
    {
        pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
    }

    function onEnd(&$ret)
    {
        // TODO: Implement onEnd() method.
    }

    function onException($e)
    {
        // TODO: Implement onException() method.
    }
}