<%@ page import="ece1779.monstersim.monster" %>
<%@ page import="java.util.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.jsp.*"%>
<%@ page import="javax.servlet.http.*" %>

<html>
	<head>
		<style>
		h1 {color:red;}
		h2 {color:blue;}
		p1 {color:black;}
		p2 {color:red;}
		p3 {color:green;}
		
		html { 
			background: url(battle_background.png) no-repeat center center fixed; 
			
			-webkit-background-size: cover; 
			-moz-background-size: cover; 
			-o-background-size: cover; 
			background-size: cover; 
		} 
		
		</style>
	</head>
	<body>
		<h1><font face="verdana">Pet Creation: Stage 2 of 3</h1>
		<title>MonsterSim: Create Your Pet!</title>
		<h2><font face="verdana">Please Select Your Pet's Avatar!</h2>
		<%
		String race = request.getParameter("race");
		if (race == null) {
 	     	response.sendRedirect("create.jsp");
		}
		%>
		<form name="input" action="pickStats.jsp" method="post">
			<p1><font face="verdana">Pick a <%=race%> Avatar!<p1><br><br>
			<input type="radio" name="pic" value="<%=race%>1"  checked='checked'><img src="icons/<%=race%>1breath.gif">
			<input type="radio" name="pic" value="<%=race%>2"><img src="icons/<%=race%>2breath.gif">
			<input type="radio" name="pic" value="<%=race%>3"><img src="icons/<%=race%>3breath.gif"><br><br>
			<p1><font face="verdana">Pick your monsters name!<p1><br>
			<input type = "text" name = "name" placeholder = "Monster Name"><br><br>
			<input type = "hidden" name ="race" value ="<%=race%>">
			<input type="submit" value="Submit"><br><br>
		</form>
		
		<form name="input" action="pet_stats.jsp" method="post">
			<input type="submit" name = "back" value="Return to homepage">
		</form>
	</body>
</html>