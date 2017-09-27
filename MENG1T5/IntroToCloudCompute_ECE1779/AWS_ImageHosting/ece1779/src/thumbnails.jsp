<%@ page import="ece1779.User" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.http.*"%> 
<%@ page import="org.apache.commons.fileupload.*"%>
<%@ page import="com.amazonaws.*"%>

<html>
	<head>
		<title>Accessing data in a database</title>
		<style>
		h1 {color:red;}
		h2 {color:blue;}
		p {color:red;}
		</style>
	</head>
	<body>
	<%
	
	if (request.getParameter("pic") != null) {
		String addr = request.getParameter("pic");
		
		DataSource dbcp = (DataSource)application.getAttribute("dbpool");
		Connection con = dbcp.getConnection();
		String sql = "SELECT key1, key2, key3, key4 from images WHERE key4 = ?";
		PreparedStatement s = con.prepareStatement(sql);
		s.setString(1,addr);
		ResultSet rs = s.executeQuery();
		String picaddr = "something";
		while (rs.next()) {
			for (int i=0;i<4;i++){
				picaddr = "https://s3.amazonaws.com/ece1779group10/" + rs.getString(i+1);
				%><img src='<%=picaddr%>'/><%
			}
			
		}
		rs.close();
		s.close();
		con.close();
	} %>
	<form name="input" action="/ece1779/src/DisplayData.jsp" method="post">
			<input type="submit" name = "back" value="Return to Images">
		</form>
	</body>
</html>