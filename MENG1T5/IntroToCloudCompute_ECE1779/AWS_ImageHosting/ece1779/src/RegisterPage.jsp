
<%@ page import="ece1779.User" %>
<%@ page import="ece1779.Form" %>
<%@ page import="java.util.*" %>

<html>
	<head>
		<style>
		h1 {color:red;}
		h2 {color:blue;}
		p1 {color:black;}
		p2 {color:red;}
		p3 {color:green;}
		</style>
	</head>
	<body>
		<h1>PickUpBox</h1>
		<title>PickUpBox: login or register by entering your username and password</title>
		<p1>Please input your username and password</p1></br></br>
		<%
		String s = (String)request.getAttribute("ACK");
		if (s != null) {
			if (!s.isEmpty() && s.equals("FAIL")) {
				%><p2>username already exists, please create a new username</p2></br></br><%
			}
			if (!s.isEmpty() && s.equals("PASS")) {
					%><p3>username successfully added!</p3></br></br><%
			}
			if (!s.isEmpty() && s.equals("PFAIL")) {
					%><p2>Passwords do not match</p2></br></br><%
			}			
		}
		%>
		<form name="input" action="${pageContext.request.contextPath}/servlet/Form" method="get">
			<input type = "text" name = "r_user" placeholder = "new username"><br/></br>
			<input type = "password" name = "r_pass" placeholder= "password"><br/></br>
			<input type = "password" name = "r_c_pass" placeholder= "confirm password"><br/></br>
			<input type="submit" name = "reg" value="Register">
		</form>
		<form name="input" action="/ece1779/src/userUI.jsp" method="post">
			<input type="submit" name = "back" value="Return to homepage">
		</form>
	</body>
</html>