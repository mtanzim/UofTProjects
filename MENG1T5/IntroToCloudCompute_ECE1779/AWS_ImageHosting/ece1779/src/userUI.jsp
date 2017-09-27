<%@ page import="ece1779.User" %>
<%@ page import="java.util.*" %>
<html>
	<head>
		<style>
		h1 {color:red;}
		h2 {color:blue;}
		p {color:black;}
		</style>
	</head>
	<body>
	    <%
	    	String  loggedIn = (String)session.getAttribute("loggedIn");
	    	if (loggedIn != null) {
	    		String site = new String("DisplayData.jsp");
	    		response.setStatus(response.SC_MOVED_TEMPORARILY);
	    		response.setHeader("Location", site);
	    	}
	    	else {
			    %>
				<h1>PickUpBox</h1>
				<title>PickUpBox: login or register by entering your username and password</title>
				<p>Please input your username and password</p>
				<form name="input" action="DisplayData.jsp" method="post">
					<input type = "text" name = "user" placeholder = "username"><br/></br>
					<input type = "password" name = "pass" placeholder= "password"><br/></br>
					<input type="submit" name = "log" value="Login">
				</form>
				<form name="input" action="RegisterPage.jsp" method="post">
					<p>If you are a new user, please register for the website</p>
					<input type="submit" name="new" value="Register"><br/>
				</form>
			<%
			}
			%>
	</body>
</html>