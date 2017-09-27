<%@ page import="ece1779.man.InstanceMetrics" %>
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
<%@ page import="com.amazonaws.auth.BasicAWSCredentials"%>
<%@ page import="com.amazonaws.services.ec2.AmazonEC2"%>
<%@ page import="com.amazonaws.services.ec2.AmazonEC2Client"%>
<%@ page import="com.amazonaws.*" %>



<html>
	<head>
		<title>Basic Manager</title>
	</head>
	<body>
	<%
		String mod = request.getParameter("mod");
		BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
	    AmazonEC2 ec2 = new AmazonEC2Client(awsCredentials);
		InstanceMetrics ti = new InstanceMetrics(); 
		PrintWriter out2 = response.getWriter();	
	
		if (mod.equals("Add Instance"))
		{    		
    		String garbage=ti.create_instance (out2,ec2);	
	        
			%><br>TOKEN:<%=ti.retrieveInstanceId() %>ENDTOO<%
		}
		else
		{
			//need to send a list of strings even if one element
			List <String> delete_instance = new ArrayList<String>();
			String instance = request.getParameter("remove_id");
			delete_instance.add(instance);
			
			ti.terminate_instance(out2,delete_instance,ec2);
			
			
			%><%="hoes" %><%
			
		}
    %>	
		<form name="input" action="/ece1779/man/managerUI.jsp" method="post">
			<input name="man_submit" type="submit" value="ManagerUI" /><br/>
		</form>
	</body>
</html>