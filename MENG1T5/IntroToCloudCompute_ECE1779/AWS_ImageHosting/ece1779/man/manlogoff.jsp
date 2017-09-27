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
		<%
			session.setAttribute("manloggedIn", null);
		%>
		<h1>Manager Login</h1>
			<p>Logged off</p>
			<form name="input" action="/ece1779/man/managerUI.jsp" method="post">
				<input name="man_submit" type="submit" value="Return to managerUI" /><br/>
			</form>
	</body>
</html>