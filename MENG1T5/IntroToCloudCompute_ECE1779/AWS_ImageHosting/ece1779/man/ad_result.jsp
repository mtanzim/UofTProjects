<%@ page import="ece1779.man.InstanceMetrics.*" %>
<%@ page import="ece1779.man.reg_reg.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="java.io.PrintWriter" %>
<%@ page import="java.io.File" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.http.*"%> 
<%@ page import="org.apache.commons.fileupload.*"%>
<%@ page import="com.amazonaws.*"%>

<html>
	<head>
		<title>Advanced Something</title>
	</head>
	<body>
	<% 
	
	int t_minus = Integer.parseInt(request.getParameter("threshold_neg"));
	int t_plus = Integer.parseInt(request.getParameter("pos_threshold"));
	double f_minus = Double.parseDouble(request.getParameter("factor_neg"));
	double f_plus = Double.parseDouble(request.getParameter("pos_factor"));
	
	if (t_minus < 0)
		t_minus = 0;
	else if (t_minus >100)
		t_minus =100;
	
	if (t_plus < 0)
		t_plus = 0;
	else if (t_plus > 100)
		t_plus =100;
	
	if (f_minus < 0)
		f_minus = 0;
	else if (f_minus >1)
		f_minus =1;
	
	if (f_plus < 1)
		f_plus = 1;
	else if (f_plus > 4)
		f_plus =4;
	//out.print("<br>CAME HERE!<br>");
	
    String path = this.getServletContext().getRealPath("/");        
    String file_name="threshold_info.txt";
    
    // store file in server
    PrintWriter writer = new PrintWriter(path+file_name,"UTF-8");

    writer.println(t_minus+","+t_plus+","+f_minus+","+f_plus);
    writer.close();
    %><p>Inserted Values <%=t_minus %> , <%=t_plus %> , <%=f_minus %> , <%=f_plus %></p>
		<form name="input" action="/ece1779/man/managerUI.jsp" method="post">
			<input name="man_submit" type="submit" value="ManagerUI" /><br/>
		</form>
	</body>
</html>