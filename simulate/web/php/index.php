<!DOCTYPE html>
<html>
<body>

<h3>Simple server</h3>
<table border="1">
  <tr>
    <th>Link</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><a href="get_date.php">getDate</a></td>
    <td>Call internal function "date".</td>
  </tr>
  <tr>
    <td><a href="test_func1.php">testFunc1</a></td>
    <td>Call user-defined function.</td>
  </tr>
  <tr>
    <td><a href="test_func2_caller.php">testFunc2</a></td>
    <td>Call user-defined function in a namespace.</td>
  </tr>
  <tr>
    <td><a href="test_method.php">testMethod</a></td>
    <td>Call user-defined class Method in a namespace.</td>
  </tr>
  <tr>
    <td><a href="simple_interceptor.php">Simple Interceptor</a></td>
    <td>An example of simple interceptor.</td>
  </tr>
</table>

<h3>Error and exception</h3>
<table border="1">
  <tr>
    <th>Link</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><a href="error1.php">error1</a></td>
    <td>Call undefined function.</td>
  </tr>
  <tr>
    <td><a href="error2.php">error2</a></td>
    <td>Syntax error. </td>
  </tr>
  <tr>
    <td><a href="exception1.php">exception1</a></td>
    <td>Exception. </td>
  </tr>
  <tr>
    <td><a href="exception2.php">exception2</a></td>
    <td>Exception in a function. </td>
  </tr>
</table>

<h3>Call remote server</h3>
<form action="curl_caller.php" method="get">
  Remote Address:  <input type="text" name="address" size="50" value="http://10.34.130.155:8090/get_date.php">   <input type="submit">
</form>

</body>
</html>