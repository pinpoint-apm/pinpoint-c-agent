<?php


namespace app\common;


class AccessRemote
{
    public function gotoUrl($url)
    {
        $ch = \curl_init();

        \curl_setopt($ch, CURLOPT_URL, $url);

        $headers = array(
            'Content-type: application/xml',
            'Authorization: gfhjui',
        );

        \curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        $tmp = \curl_exec($ch);
        curl_close($ch);
        var_dump($tmp);
//        $this->callExample();
        return $tmp;
    }

    private function callExample()
    {
        $ch = \curl_init();

        \curl_setopt($ch, CURLOPT_URL, "exmaple.com");

        $tmp = \curl_exec($ch);

        curl_close($ch);
        return $tmp;
    }
}