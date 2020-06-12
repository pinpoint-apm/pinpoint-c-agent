<?php
/**
 * User: eeliu
 * Date: 1/25/19
 * Time: 10:18 AM
 */

//echo ";;;";
//var_dump(eval('$str="sdfasfdasfd";return $str;'));

$a =10;
$b=13;


function foo($a,$b){

    $onbefore=<<<XXX
use FOO;
\$foo = new Foo();
\$foo->onBefore(\$a);
return \$foo;
XXX;
    eval($onbefore);

$onEnd=<<<END
\$foo->onEnd();
END;
    eval($onEnd);

}

foo(12,44);



