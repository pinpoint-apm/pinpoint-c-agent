<?php
/**
 * User: eeliu
 * Date: 1/3/19
 * Time: 7:17 PM
 */

namespace Foo;


//class Pinpoint_PDO extends \PDO
//{
//
//}
//
//
//class test
//{
//    public function out(){
//
//        try {
//            $dbh = new Pinpoint_PDO('mysql:host=localhost;dbname=test', $user, $pass);
//            foreach($dbh->query('SELECT * from FOO') as $row) {
//                print_r($row);
//            }
//            $dbh = null;
//        } catch (PDOException $e) {
//            print "Error!: " . $e->getMessage() . "<br/>";
//            die();
//        }
//    }
//}
//
//
//$tt = new test();
//$tt->out();
//
//function add_field($call){
//    $call->header="afasfadf";
//}
//
//class Foo{
//    public function __construct()
//    {
//        add_field($this);
//    }
//    public function output(){
//        print $this->header;
//    }
//}
//
//$f = new Foo();
//$f->output();


//function foo_test_much(...$args)
//{
//    var_dump($args);
//}
//
//function call($a,...$args)
//{
//    print $a;
//    foo_test_much($args);
//}
//
//call(1,3,4,'456','356356');



////
//$ch = curl_init();
//
//// ÉèÖÃURLºÍÏàÓ¦µÄÑ¡Ïî
//curl_setopt($ch, CURLOPT_URL, "http://www.example.com/");
//
////$ch->url = "http://www.example.com/";
//
//curl_setopt($ch, CURLOPT_HEADER, 0);
//
//// ×¥È¡URL²¢°ÑËü´«µÝ¸øä¯ÀÀÆ÷
//curl_exec($ch);
////echo $ch->url;
//// ¹Ø±ÕcURL×ÊÔ´£¬²¢ÇÒÊÍ·ÅÏµÍ³×ÊÔ´
//curl_close($ch);



//
//class Foo{
//    protected function output()
//    {
//        echo "Foo::output";
//    }
//
//    function test_test()
//    {
//        $this->test();
//    }
//}
//
//class SupFoo extends  Foo {
//    function output()
//    {
//        parent::output();
//    }
//
//    function test()public function __construct()
//    {

//    {
//        $this->output();
//    }
//}
//
//$s = new SupFoo();
//$s->test();


//require_once "Life.php";
//echo "start \n";
//echo "end\n";


//class Test{
//
//    public function none(){
//
//    }
//
//    public function __construct()
//    {
//        $this->do = $this->none; #fdf
//    }
//
//    public function do(){
//        echo "do";
//    }
//}
//
//### dfasdfasdfafds
//$test = new Test();
//$test->do();
//
//use function spec\Prophecy\Promise\functionCallbackFirstArgument;
//
//$test = require_once "Life.php";
////$life->do();
//
//
////
////$life1 = new Life();
////$life2 = new Life();
////$life3 = new Life();
////$life4 = new Life();
////$life5 = new Life();
//
//function shutdown()
//{
//
//    echo " --- down ---";
////    exit();
//}
//
//function erro_handler($e)
//{
//    echo "erro_handler";
//    register_shutdown_function("Foo\\shutdown");
//    if($e instanceof \Exception)
//        throw $e;
//    restore_error_handler();
//    restore_exception_handler();
//}
//
//function excep_handler($e)
//{
//    restore_exception_handler();
//
//}
//
//
////register_shutdown_function("Foo\shutdown");
////register_shutdown_function("Foo\shutdown");
////set_error_handler("Foo\\erro_handler");
////set_exception_handler("Foo\\erro_handler");
//
//$ince = Test::create();
//
//function test()
//{
//    try{
//        $life1 = new Life();
//        throw new \Exception("asdfsdaf");
////        trigger_error("xxxx",E_USER_ERROR);
//    }catch (\Exception $e){
////        $life1->do();
////        echo $e;
//        throw $e;
//    }
//}



//try{
//    $life1 = new Life();
//    $life2 = new Life();
//    $life3 = new Life();
//    $life4 = new Life();
//    $life5 = new Life();

//test();
//    4/0;
//    throw new \Exception("fvgbh");
//}catch (\Exception $e){
//
//}


//echo ini_get('pinpoint_php.CollectorHost')."\n";
//echo ini_get('pinpoint_php.SendSpanTimeOutMs')."\n";
//var_dump(ini_get('pinpoint_php.UnitTest"'));
//
//echo ini_get('pinpoint_php.AppId')."\n";
//
//echo ini_get('pinpoint_php.StartTime');

//var_dump(ini_set('pinpoint_php.StartTime',10));
//
//echo ini_get('pinpoint_php.StartTime');
//
//function curl_exec($args){
//    \curl_exec($args);
//}
//
//
//curl_exec("ff");

//
//function test($n)
//{
//    for($i = 0;$i<$n;$i++)
//    {
//        yield $i;
//    }
//}
//
//var_dump(test(2));
//
//class WapperGenerator implements \Iterator{
//    private $gernator;
//    function __construct($gerator)
//    {
//        assert($gerator instanceof \Generator);
//        $this->gernator = $gerator;
//    }
//
//    function __destruct()
//    {
//        // TODO: Implement __destruct() method.
//    }
//
//    /**
//     * Return the current element
//     * @link https://php.net/manual/en/iterator.current.php
//     * @return mixed Can return any type.
//     * @since 5.0.0
//     */
//    public function current()
//    {
//        return $this->gernator->current();
//    }
//
//    /**
//     * Move forward to next element
//     * @link https://php.net/manual/en/iterator.next.php
//     * @return void Any returned value is ignored.
//     * @since 5.0.0
//     */
//    public function next()
//    {
//        echo "call next ";
//       return  $this->gernator->next();
//    }
//
//    /**
//     * Return the key of the currentjw element
//     * @link https://php.net/manual/en/iterator.key.php
//     * @return mixed scalar on success, or null on failure.
//     * @since 5.0.0
//     */
//    public function key()
//    {
//        return  $this->gernator->key();
//    }
//
//    /**
//     * Checks if current position is valid
//     * @link https://php.net/manual/en/iterator.valid.php
//     * @return boolean The return value will be casted to boolean and then evaluated.
//     * Returns true on success or false on failure.
//     * @since 5.0.0
//     */
//    public function valid()
//    {
//        return  $this->gernator->valid();
//    }
//
//    /**
//     * Rewind the Iterator to the first element
//     * @link https://php.net/manual/en/iterator.rewind.php
//     * @return void Any returned value is ignored.
//     * @since 5.0.0
//     */
//    public function rewind()
//    {
//        return  $this->gernator->rewind();
//    }
//}
//
//$set= new  WapperGenerator(test(2));
//
//foreach ($set as $i)
//{
//    echo $i."\n";
//}


//// Create a cURL handle
//$ch = curl_init('http://www.example.com/');
//
//// Execute
//curl_exec($ch);
//
//// Check if any error occurred
////if (!curl_errno($ch)) {
//$info = curl_getinfo($ch); //CURLINFO_EFFECTIVE_URL);
//print_r($info);
//echo "55555 $info 55555";
//    print_r($info);
//    echo 'Took ', $info['total_time'], ' seconds to send a request to ', $info['url'], "\n";
//}

// Close handle
//curl_close($ch);
//
//class TestSta{
//    public static $bc = 10;
//    public function __construct()
//    {
////        TestSta::$bc = 10;
//    }
//    public function test($a,$b){
//        echo __METHOD__;
//    }
//}
//
//class TestSun extends TestSta{
//
//    public function test(...$etc)
//    {
////        parent::test($args); // TODO: Change the autogenerated stub
//    }
//}
//
//$t = new TestSta();
//$t::$bc = 11;
//
//
//$a = new TestSun();
////
////print $a::$bc;
//echo $a->test(1,3);



//$p = new \ReflectionMethod("PDO","query");
//
//\ReflectionMethod::export($p);
//echo \ReflectionMethod::export("PDO","query",true);

//va \Reflection::export($pdo);
//
//
//$pdo =new  \ReflectionClass('PDO');
//$query = $pdo->getMethod("prepare");
//echo $query->getNumberOfParameters();
//\Reflection::export($pdo);


$p = new \ReflectionFunction("date");

foreach ($p->getParameters() as $arg) {
if($arg->isOptional())
{
    echo $arg->getDefaultValue();
}

    var_dump($arg);
    echo "-\n";
}
//
//
//class PDO extends \PDO
//{
//    public function query()
//    {
//        $args = func_get_args();
//        return call_user_func_array(array('parent', __FUNCTION__),$args);
//    }
//    function exec($query)
//    {
//    }
//}
//
//$dbh = new PDO('mysql:ho
//st=dev-test-mysql;dbname=test', 'root', 'root');
//foreach($dbh->query('SELECT * from migrations') as $row) {
//    print_r($row);
//}

