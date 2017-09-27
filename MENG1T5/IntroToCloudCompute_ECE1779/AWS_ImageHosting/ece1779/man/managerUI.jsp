<%@ page import="java.util.*" %>
<html>
	<head>
		<title>Manager Login</title>
	</head>
	<body>
	<%
		String  loggedIn = (String)session.getAttribute("manloggedIn");
		if (loggedIn == null)
		{
		%>
			<h1>Manager Login</h1>
			<form name="input" action="/ece1779/man/manhelper.jsp" method="get">
				<p>Username:<input name="man_user" type="text" /></p>
				<p>Password:<input name="man_pass" type="password" /></p>
				<input name="man_submit" type="submit" value="Submit" /><br/>
			</form>
		<%
		}
		else{
			%>
			<h1>Manager Login</h1>
			<p>Logged in as Manager</p>
			<form name="input" action="/ece1779/man/basic_man.jsp" method="post">
				<input name="man_submit" type="submit" value="Basic Manager" /><br/>
			</form>
			<form name="input" action="/ece1779/man/ad_man.jsp" method="post">
				<input name="man_submit" type="submit" value="Advanced Manager" /><br/>
			</form>
			<form name="input" action="/ece1779/servlet/InstanceMetrics" method="get">
				<input name="man_submit" type="submit" value="Enable Autoscale" /><br/>
			</form>
			<form name="input" action="/ece1779/man/DELETEALL.jsp" method="post">
				<input name="man_submit" type="submit" value="Delete ALL" /><br/>
			</form>
			<form name="input" action="/ece1779/man/manlogoff.jsp" method="post">
				<input name="man_submit" type="submit" value="Logout" /><br/>
			</form>
			<%
		}%>
	</body>
</html>
