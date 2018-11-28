<?php
namespace testNameSpace;
function test_func2($arg1, $arg2)
{
  return sprintf("this is test_func2 in namespace testNameSpace: arg1=%s, arg2=%s", (string)$arg1, (string)$arg2);
}

class TestClass
{
  public function getVarAdd($arg1)
  {
    return $this->num + $arg1;
  }

  var $num;
};

?>