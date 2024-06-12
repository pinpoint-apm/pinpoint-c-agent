<%--
  Created by IntelliJ IDEA.
  User: chenguoxi
  Date: 8/24/17
  Time: 11:21 AM
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<html>
<head>
    <title>JavaCallApp</title>
</head>
<body>
    <h3>Call remote server</h3>
    <form action="call.pinpoint" method="get">
        Remote Address:  <input type="text" name="address" size="50" value="http://10.34.130.155:8090/get_date.php">   <input type="submit">
    </form>
</body>
</html>
