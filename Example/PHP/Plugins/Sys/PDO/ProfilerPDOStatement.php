<?php
#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------

namespace Plugins\Sys\PDO;
use function GuzzleHttp\Psr7\normalize_header;
use PDO;
use Plugins\Common\InstancePlugins;

class ProfilerPDOStatement extends \PDOStatement
{
    protected $_instance ;
    protected $_name_list;
    protected $name;
    public function __construct(&$instance)
    {
        $this->_instance = &$instance;
        $this->_name_list=[
            'fetchAll','fetch','nextRowset','getColumnMeta','fetchObject','execute'
        ];
    }

    protected function onBefore()
    {

        pinpoint_start_trace();
        pinpoint_add_clue(INTERCEPTER_NAME,$this->name);

    }

    protected function onEnd(&$ret)
    {
        pinpoint_end_trace();
    }

    protected function onException($e)
    {
        pinpoint_add_clue(ADD_EXCEPTION,$e->getMessage());
    }


    public function bindValue($parameter, $value, $data_type = PDO::PARAM_STR)
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->profiler(__FUNCTION__,$args);
    }

    public function closeCursor()
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->profiler(__FUNCTION__,$args);
    }

    public function columnCount()
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function debugDumpParams()
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function errorCode()
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function errorInfo()
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function execute($input_parameters = null)
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->profiler(__FUNCTION__,$args);
    }

    public function fetch($fetch_style = null, $cursor_orientation = PDO::FETCH_ORI_NEXT, $cursor_offset = 0)
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->profiler(__FUNCTION__,$args);
    }

    public function fetchAll($how = PDO::FETCH_CLASS, $class_name = null, $ctor_args = null)
    {
        $args = \pinpoint_get_func_ref_args();
        return $this->profiler(__FUNCTION__,$args);
    }

    public function fetchColumn($column_number = 0)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function fetchObject($class_name = "stdClass", $ctor_args = null)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function getAttribute($attribute)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function getColumnMeta($column)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function nextRowset()
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function rowCount()
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function setAttribute($attribute, $value)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function setFetchMode($mode,$parames=null)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }


    public function bindParam ($parameter, &$variable, $data_type = PDO::PARAM_STR, $length = null, $driver_options = null)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    public function bindColumn ($column, &$param, $type = null, $maxlen = null, $driverdata = null)
    {
       $args = \pinpoint_get_func_ref_args();
       return $this->profiler(__FUNCTION__,$args);
    }

    private function profiler($name,&$arguments)
    {
        if(!in_array($name ,$this->_name_list))
        {
            return call_user_func_array([$this->_instance,$name],$arguments);
        }else{
            try{
                $this->name = "PDOStatement::".$name;
                $this->args = &$arguments;
                $this->onBefore();
                $ret = call_user_func_array([$this->_instance,$name],$arguments);
                $this->onEnd($ret);
                return $ret;
            }catch (\Exception $e){
                $this->onException($e);
                $ret = null;
                $this->onEnd($ret);
                throw $e;
            }
        }

    }

}