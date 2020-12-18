<?php
/******************************************************************************
 * Copyright 2020 NAVER Corp.                                                 *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 ******************************************************************************/
require_once "Common/PluginsDefines.php";

// intercept all date_xxxx, part of php core
require_once "Sys/date/date.php";

// intercept all curl_xxxx, if curl extension is available
if (function_exists('curl_exec'))
{
    require_once "Sys/curl/curl.php";
}

if (function_exists('mysqli_connect'))
{
    require_once "Sys/mysqli/mysqli.php";
}