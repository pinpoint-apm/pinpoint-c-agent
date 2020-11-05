<?php

namespace app;
use GuzzleHttp\Client;

class TestGuzzle
{
    private $client = null;
    public function __construct()
    {
        $this->client = new Client([
            // Base URI is used with relative requests
            'base_uri' => 'https://example.org',
            // You can set any number of default request options.
            'timeout'  => 2.0,
        ]);
    }

    public function gotoV()
    {
        $response = $this->client->request('GET', '/');
        return $response->getStatusCode();
    }

    public function gotofoo()
    {
        $response = $this->client->request('GET', 'foo',['query'=>['foo'=>'bar']]);
        return $response->getStatusCode();
    }


}