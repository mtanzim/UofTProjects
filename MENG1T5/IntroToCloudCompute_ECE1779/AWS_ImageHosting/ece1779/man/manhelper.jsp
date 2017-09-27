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
		String manUser = application.getInitParameter("manUser");
		String manPassword = application.getInitParameter("manPassword");
		String user = (String)request.getParameter("man_user");
		String pass = (String)request.getParameter("man_pass");
		if ((user.equals(manUser) )&& (pass.equals(manPassword))) {
			session.setAttribute("manloggedIn", "yes");
		%>
		<h1>Manager Login</h1>
			<p>Logged in as Manager </p>
			<form name="input" action="/ece1779/man/basic_man.jsp" method="post">
				<input name="man_submit" type="submit" value="Basic Manager" /><br/>
			</form>
			<form name="input" action="/ece1779/man/ad_man.jsp" method="post">
				<input name="man_submit" type="submit" value="Advanced Manager" /><br/>
			</form>
			<form name="input" action="/ece1779/man/DELETEALL.jsp" method="post">
				<input name="man_submit" type="submit" value="Delete ALL" /><br/>
			</form>
			<form name="input" action="/ece1779/man/manlogoff.jsp" method="post">
				<input name="man_submit" type="submit" value="Logout" /><br/>
			</form>
		<%	
		}
		else {
			session.setAttribute("manloggedIn", null);
		%>
		<h1>Manager Login</h1>
			<p>Wrong Username or Password</p>
			<form name="input" action="/ece1779/man/managerUI.jsp" method="post">
				<input name="man_submit" type="submit" value="Retry" /><br/>
			</form>
		<%	
		}%>
	</body>
</html>