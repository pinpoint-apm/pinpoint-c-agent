<?php

namespace app;
use MongoDB;

class TestMongo
{
    private $mclient;
    private $db;

    public function __construct($url)
    {
        $this->mclient = new MongoDB\Client($url);
        $this->db = $this->mclient->selectDatabase("testDB");
    }

    public function testAll()
    {
        $collection = $this->db->moon;
        $result = $collection->insertOne(['id'=>1,'_age'=>10,'key'=>'a']);
        $result->getInsertedCount();
        $find_ret = $collection->findOne(['_age'=>10]);
        var_dump( $find_ret);

        $update_ret = $collection->updateMany(['_age'=>10],['$set'=>['key'=>'akey']]);
        var_dump( $update_ret);

        $delete_ret = $collection->deleteMany(['_age'=>10]);
        var_dump($delete_ret);
    }

}
