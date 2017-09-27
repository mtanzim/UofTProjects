<%@ page import="ece1779.man.InstanceMetrics.*" %>
<%@ page import="ece1779.man.reg_reg.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.http.*"%> 
<%@ page import="org.apache.commons.fileupload.*"%>
<%@ page import="com.amazonaws.*"%>
<%@ page import="java.io.PrintWriter" %>
<%@ page import="java.io.File" %>
<%@ page import="java.io.InputStream" %>
<%@ page import="java.io.*" %>
<%@ page import="java.nio.charset.Charset" %>

<html>
	<head>
		<title>Basic Manager</title>
	</head>
	<body>
		<h1>Basic Manager</h1>
		<p>Worker CPUUtilization [%]</p>
		<%
		InputStream fis;
    	BufferedReader br;
    	String line;
    	
    	String path = this.getServletContext().getRealPath("/");  
    	fis = new FileInputStream(path+"instance_info.txt");
    	br= new BufferedReader(new InputStreamReader(fis,Charset.forName("UTF-8")));
    	
    	while((line=br.readLine())!=null)
    	{
    		%><br><%=line %><%
    	}
    	
    	br.close();
    	br=null;
    	fis=null;
    	%>
    	<br><br>
		<form name="input" action="basic_result.jsp" method="get">
			<p><input name="mod" type="submit" value="Add Instance" /></p>
			<p><input maxlength="20" name="remove_id" type="text" /><input name="mod" type="submit" value="Remove Instance" /></p>
		</form>
		<form name="input" action="/ece1779/man/managerUI.jsp" method="post">
			<input name="man_submit" type="submit" value="ManagerUI" /><br/>
		</form>
	</body>
</html>