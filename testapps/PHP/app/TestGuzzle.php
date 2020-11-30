<?php

namespace app;
use GuzzleHttp\Client;
use Psr\Http\Message\ResponseInterface;
use GuzzleHttp\Exception\RequestException;
/**
 *  copy the sample code from https://guzzle-cn.readthedocs.io/
 */
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

    public function testAsync()
    {
        $promise  = $this->client->requestAsync('GET', 'foo',['query'=>['foo'=>'bar']]);
        $promise->then(
            function (ResponseInterface $res) {
                echo $res->getStatusCode() . "\n";
            },
            function (RequestException $e) {
                echo $e->getMessage() . "\n";
                echo $e->getRequest()->getMethod();
            }
        );
    }


}