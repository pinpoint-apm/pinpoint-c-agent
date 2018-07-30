<?php
/**
 * Copyright 2018 NAVER Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

function test_cumsum1($n)
{
    if ($n == 1) return 1;
    return $n + test_cumsum1($n - 1);
}

function test_cumsum2($n)
{
    if ($n == 1) return 1;
    return $n + test_cumsum2($n - 1);
}

function test_cumsum3($n)
{
    if ($n < 1) throw new Exception("n < 1");
    $tmp = $n;
    try
    {
        $tmp += test_cumsum3($n-1);
    }
    catch( Exception $e)
    {

    }
    return $tmp;
}

function test_cumsum4($n)
{
    if ($n < 1) throw new Exception("n < 1");
    $tmp = $n;
    try
    {
        $tmp += test_cumsum4($n-1);
    }
    catch( Exception $e)
    {

    }
    return $tmp;
}

echo test_cumsum1(3);
echo test_cumsum2(3);
echo test_cumsum3(3);
echo test_cumsum4(3);
?>
