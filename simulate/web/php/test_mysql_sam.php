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

function db_connect($server='dev-db-wordpress:3306', $user='root', $password='123456', $database='wordpress'){
	try{
		$link = new mysqli($server,$user,$password,$database);
		if(mysqli_connect_error()){
			echo mysqli_connect_error();
		}
	}catch(Exception $e){
		echo $e->getMessage();
		exit;
	}

	return $link;
}

$link = db_connect('dev-db-wordpress:3306','root','123456','wordpress');

$sql = "select * from wp_options where option_name='home'";

$results = $link->query($sql);
echo $results->num_rows;
$link->close();

?>