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
namespace Plugins\Sys\PDO;

use Plugins\Common\Candy;

class PreparePlugin extends Candy

{
    function onBefore()
    {
        // todo stp, should follow the dsn
        $dbInfo = $this->parseDb($this->who->dsn);
        pinpoint_add_clue(PP_SERVER_TYPE,PP_MYSQL);
        pinpoint_add_clue(PP_SQL_FORMAT, $this->args[0]);
        pinpoint_add_clue(PP_DESTINATION,$dbInfo['host']);
    }
    function onEnd(&$ret)
    {
        $origin = $ret;
        $ret = new ProfilerPDOStatement($origin);
    }

    function onException($e)
    {
        pinpoint_add_clue(PP_ADD_EXCEPTION,$e->getMessage());
    }

    function parseDb($dsn){

        $db_url =  parse_url($dsn);
        parse_str(str_replace(';','&',$db_url['path']),$dbInfo);

        if($db_url['scheme'] == 'sqlite'){ // treat sqllite as mysql
            $dbInfo['host'] = 'localhost-sqlite';
        }

        $dbInfo['scheme']= $db_url['scheme'];

        return $dbInfo;
    }
}