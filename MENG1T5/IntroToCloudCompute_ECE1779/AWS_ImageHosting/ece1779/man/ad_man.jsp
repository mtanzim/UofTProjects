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
<html>
	<head>
		<title>Advanced Manager</title>
	</head>
	<body>
		<h1>Advanced Manager</h1>
		<form name="input" action="ad_result.jsp" method="post">
			<p><input maxlength="6" name="threshold_neg" type="number" value="" />
			Set Threshold<input maxlength="6" name="pos_threshold" type="number" value="" /></p>
			<p><input maxlength="10" name="factor_neg" type="double" value="" />
			Set Factor  <input maxlength="10" name="pos_factor" type="double" value="" /></p>
			<p><input name="update_mon" type="submit" value="Submit" /></p>
		</form>
	</body>
</html>