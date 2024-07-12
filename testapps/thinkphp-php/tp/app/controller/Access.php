<?php

namespace app\controller;

use app\BaseController;

use think\facade\Db;

class Access extends BaseController
{
    public function index()
    {
        return 'access';
    }

    public function db()
    {
        $user = Db::table('employees')->where('emp_no', 10016)->cache(2)->find();
        $first_name = $user['first_name'];
        $last_name = $user['last_name'];
        return "$first_name.$last_name";

    }
}