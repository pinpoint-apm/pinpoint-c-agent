<?php
namespace SimplePHP;

use Pinpoint\Plugins\PinpointPerRequestPlugins;
use Pinpoint\Common\UserFrameworkInterface;
use Pinpoint\Common\AspectClassHandle;
use MongoDB;

class RequestPlugin extends PinpointPerRequestPlugins implements UserFrameworkInterface
{
    public function __construct()
    {
        parent::__construct();
    }
    public function joinedClassSet(): array
    {
        $cls = [];

        $classHandler = new AspectClassHandle(MongoDB\Client::class);
        $classHandler->addJoinPoint('__construct', \Pinpoint\Plugins\MongoPlugin\MongoPlugin::class);
        $cls[] = $classHandler;

        $classHandler = new AspectClassHandle(MongoDB\Collection::class);
        $classHandler->addJoinPoint('insertOne', \Pinpoint\Plugins\MongoPlugin\MongoPlugin::class);
        $classHandler->addJoinPoint('updateOne', \Pinpoint\Plugins\MongoPlugin\MongoPlugin::class);
        $classHandler->addJoinPoint('deleteMany', \Pinpoint\Plugins\MongoPlugin\MongoPlugin::class);
        $classHandler->addJoinPoint('find', \Pinpoint\Plugins\MongoPlugin\MongoPlugin::class);
        $cls[] = $classHandler;

        return $cls;
    }
    public function userFindClass(&$loader): callable
    {
        return NULL;
    }
}