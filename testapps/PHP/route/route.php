<?php
// +----------------------------------------------------------------------
// | ThinkPHP [ WE CAN DO IT JUST THINK ]
// +----------------------------------------------------------------------
// | Copyright (c) 2006~2018 http://thinkphp.cn All rights reserved.
// +----------------------------------------------------------------------
// | Licensed ( http://www.apache.org/licenses/LICENSE-2.0 )
// +----------------------------------------------------------------------
// | Author: liu21st <liu21st@gmail.com>
// +----------------------------------------------------------------------

// Route::get('think', function () {
//     return 'hello,ThinkPHP5!';
// });

// Route::get('hello/:name', 'index/hello');

return [
    '/get_date' => 'index.php/index/Index/get_date',
    '/test_func1' => 'index.php/index/Index/test_func1',
    '/test_func2_caller' => 'index.php/index/Index/test_func2_caller',
    '/test_inherit_func' => 'index.php/index/Index/test_inherit_func',
    '/test_generator_func' => 'index.php/index/Index/test_generator_func',
    '/test_abstract_func' => 'index.php/index/Index/test_abstract_func',
    '/test_interface_func' => 'index.php/index/Index/test_interface_func',
    '/test_clone' => 'index.php/index/Index/test_clone',
    '/test_recursion' => 'index.php/index/Index/test_recursion',
    '/test_anonymous' => 'index.php/index/Index/test_anonymous',
    '/test_static' => 'index.php/index/Index/test_static',
    '/test_final_class' => 'index.php/index/Index/test_final_class',
    '/test_final_func' => 'index.php/index/Index/test_final_func',
    '/test_trait' => 'index.php/index/Index/test_trait',
    '/test_callback' => 'index.php/index/Index/test_callback',
    '/test_level' => 'index.php/index/Index/test_level',
    '/test_args' => 'index.php/index/Index/test_args',
    '/test_return' => 'index.php/index/Index/test_return',
//// Test Exception
    '/test_call_undefined_function' => 'index.php/index/Index/test_call_undefined_function',
    '/test_uncaught_exception' => 'index.php/index/Index/test_uncaught_exception',
    '/test_caught_exception' => 'index.php/index/Index/test_caught_exception',
    '/test_exception_recursion' => 'index.php/index/Index/test_exception_recursion',
///////////////curl
    '/test_curl' => 'index.php/index/Index/test_curl',
//////////////PDO
    '/test_pdo' => 'index.php/index/Index/test_pdo',
//////////////Redis
    '/test_redis' => 'index.php/index/Index/test_redis',
///////////////guzzle
    '/test_guzzle' => 'index.php/index/Index/test_guzzle',
///////////////mongo
    '/test_mongo' => 'index.php/index/Index/test_mongo',
];
