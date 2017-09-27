<%@ page import="ece1779.User" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="javax.servlet.*"%>


<%@ page import="javax.servlet.http.*"%> 
<html>
	<body>
		<% 
			LinkedList<User> user_ll = new LinkedList<User>();
			
			if((LinkedList<User>)request.getSession().getAttribute("users") != null)	
				user_ll = (LinkedList<User>)request.getSession().getAttribute("users");//retrieve users servlet context
			
			User tempuser = new User();
			tempuser.setUserid(request.getParameter("user"));
			tempuser.setPassword(request.getParameter("pass"));
	
			user_ll.add(tempuser);
			Connection con = null;
			
			try {
				//get DB connection from pool
				DataSource dbcp =(DataSource)application.getAttribute("dbpool");
				con = dbcp.getConnection();
				
				String query = "INSERT INTO users (login, password) " + "VALUES ('"+request.getParameter("user")+"','"+request.getParameter("pass")+"')";
			
				//Execute SQL query
				Statement stmt = con.createStatement();
				stmt.executeUpdate(query);
				stmt.close();
			%>
			
			<%= ("new user successfully added to the database!")%><br/><br/>
			
			<%	
    		} catch( SQLException e ){
        		e.printStackTrace();
    		}
    		
    		try {
            	if (con != null) con.close();
        	} catch ( SQLException e ) {
            		System.err.println( "Problems when closing connection" );
            		e.printStackTrace();
        	}
			
			%>
			
			<%= ("new user added!")%><br/><br/>
			
			<%
			for (int i=0; i < user_ll.size(); i++)
			{
			%>
			<%=	("user: " + user_ll.get(i).getUserid()) %>
			<%=	(" |  pass: " + user_ll.get(i).getUserid()) %> <br/>
			<%
			} 
			request.getSession().setAttribute("users", user_ll);//put back new user_ll linkedlist in servletcontext
			%>
		<form name="input" action="userUI.jsp" method="get">
			<input type = "submit" value = "Return"></input>
		</form>
		<form name="input" action="DisplayData.jsp" method="get">
			<input type = "submit" value = "Display"></input>
		</form>
	</body>
</html>