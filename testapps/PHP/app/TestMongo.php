<?php
namespace app;
use MongoDB\Client as mClient;

class TestMongo
{
    private $mongoClient ;

    public function __construct($url)
    {
        $this->mongoClient = new mClient($url);
    }

    public function testAll()
    {
        $testdb = $this->mongoClient->testDB->moon;
        $cursor = $testdb->find(['id'=>1]);
        foreach ($cursor as $key)
        {
            var_dump($key);
        }

        $ret = $testdb->insertOne([
            'id'=>2,
            'age'=>1120,
            'height'=>1120
        ]);

        var_dump($ret->getInsertedId());


        $ret = $testdb->updateOne(['id'=>2],['$set'=>['height'=>120]]);
        var_dump($ret->getMatchedCount());


        $ret = $testdb->deleteMany(['age'=>1120]);
        var_dump($ret->getDeletedCount());
    }

}