<%@ page import="ece1779.User" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.jsp.*"%>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="org.apache.commons.fileupload.*"%>
<%@ page import="com.amazonaws.*"%>

<html>
	<head>
		<title>Accessing data in the database</title>
		<style>
		h1 {color:red;}
		h2 {color:blue;}
		h3 {color:black;}
		p {color:red;}
		p1 {color:green;}
		p2 {color:blue;}
		</style>
	</head>
	<body>
	<%
	
	String  loggedIn = (String)session.getAttribute("loggedIn");
	if (loggedIn == null) {
		try {
			DataSource dbcp = (DataSource)application.getAttribute("dbpool");
			Connection con = dbcp.getConnection();
			
			//System.out.println("got connection");
			String username = request.getParameter("user");
			String password = request.getParameter("pass");
			String sql = "SELECT login, password, id from users WHERE login = ?";
			PreparedStatement s = con.prepareStatement(sql);
			s.setString(1,username);
			ResultSet rs = s.executeQuery();
			if (rs.next()) {
				if (rs.getString(2).equals(password)) {
					%><p1>Correct username and password</p1>				
					
					<%
					String v_username = rs.getString(1);
					String v_password = rs.getString(2);
					v_username.replace(" ", "_");
					v_password.replace(" ", "_");
					loggedIn = rs.getString(1) + " " + rs.getString(2) + " " + rs.getString(3);
					session.setAttribute("loggedIn", loggedIn);
				}
				else {
					%><p>Incorrect password</p><%			
				}
			}
			else {
				%><p>username does not exist</p><%
			}
			rs.close();
			s.close();
			con.close();
		}
		catch (SQLException e2) {
		  	// Exception when executing java.sql related commands, print error message to the console
		  	System.out.println(e2.toString());
		}
		catch (Exception e3) {
		  	// other unexpected exception, print error message to the console
		  	System.out.println(e3.toString());
		}
	}
	
	if (loggedIn != null) {
		String[] string = loggedIn.split(" ");
		String v_username = string[0];
		String v_password = string[1];
		String v_id = string[2];
		v_username.replace("_", " ");
		v_password.replace("_", " ");
		%>
		<h3>Logged in as <%=v_username%> </h3>
		<%
		DataSource dbcp = (DataSource)application.getAttribute("dbpool");
		Connection con = dbcp.getConnection();

		String sql = "SELECT login, password, id from users WHERE login = ?";
		PreparedStatement s = con.prepareStatement(sql);
		s.setString(1,v_username);
		ResultSet rs = s.executeQuery();
		if (rs.next()) {
			if (rs.getString(2).equals(v_password)) { //passwordcheck again needed?
				%>
				<form method='post' action='thumbnails.jsp'>
				<%
				String getthumb = "SELECT key4 from images WHERE userId = ?";//get thumbnals only for specific user
				PreparedStatement s2 = con.prepareStatement(getthumb);
				s2.setString(1,rs.getString(3));
				ResultSet rsthumb = s2.executeQuery();
				while (rsthumb.next()) {
					String addr = "https://s3.amazonaws.com/ece1779group10/" + rsthumb.getString(1);
					%><button name ="pic" type ="submit" value ="<%=rsthumb.getString(1)%>"><img src='<%=addr%>' border="0"></button>
					<%
				}
				%></form><%
			}
		}
		rs.close();
		s.close();
		con.close();
	}
	%>
	<form name="input" action="fileUpload.html" method="post">
						<input type="submit" name = "upload" value="Upload Image">
					</form>
	<form name="input" action="userUI.jsp" method="get">
		<input type="submit" name = "return" value="Return to homepage">
	</form>
	<form name="input" action="${pageContext.request.contextPath}/servlet/Logout" method="get">
		<input type="submit" name = "logout" value="Logout">
	</form>
	</body>
</html>