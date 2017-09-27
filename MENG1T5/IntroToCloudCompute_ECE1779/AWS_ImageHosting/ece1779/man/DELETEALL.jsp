<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.jsp.*"%>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="java.sql.*" %>
<%@ page import="javax.sql.DataSource" %>
<%@ page import="org.apache.commons.fileupload.*"%>
<%@ page import="com.amazonaws.*"%>
<%@ page import="com.amazonaws.auth.BasicAWSCredentials" %>
<%@ page import="com.amazonaws.services.s3.AmazonS3"%>
<%@ page import="com.amazonaws.services.s3.AmazonS3Client"%>
<%@ page import="com.amazonaws.services.s3.model.CannedAccessControlList"%>
<%@ page import="com.amazonaws.services.s3.model.GetObjectRequest"%>
<%@ page import="com.amazonaws.services.s3.model.ListObjectsRequest"%>
<%@ page import="com.amazonaws.services.s3.model.PutObjectRequest"%>
<%@ page import="com.amazonaws.services.s3.model.Bucket"%>
<%@ page import="com.amazonaws.services.s3.model.S3Object"%>
<%@ page import="com.amazonaws.services.s3.model.ObjectListing"%>
<%@ page import="com.amazonaws.services.s3.model.S3ObjectSummary"%>

<html>
	<head>
		<style>
		h1 {color:red;}
		h2 {color:blue;}
		p1 {color:black;}
		p2 {color:red;}
		p3 {color:green;}
		</style>
	</head>
	<body>
		<%
		DataSource dbcp = (DataSource)application.getAttribute("dbpool");
		Connection con = dbcp.getConnection();
		//s3
		BasicAWSCredentials awsCredentials = (BasicAWSCredentials)this.getServletContext().getAttribute("AWSCredentials");

		AmazonS3 s3 = new AmazonS3Client(awsCredentials);
	    
	    String bucketName = "ece1779group10";

	        try {
	        	
	        	String sql = "SELECT key1, key2, key3, key4 from images";
	    		PreparedStatement s = con.prepareStatement(sql);
	    		ResultSet rs = s.executeQuery();
	    		while (rs.next()) {
	    			for (int i=0;i<4;i++){
	    				s3.deleteObject(bucketName,rs.getString(i+1));
	    			}
	    		}
	    		
	    		String sql2 = "DELETE from users";
	    		Statement s2 = (Statement)con.createStatement();
	    		s2.execute(sql2);
	    		
	    		rs.close();
	    		s.close();
	    		s2.close();
	    		con.close();

	        } catch (AmazonServiceException ase) {
	            out.println("Caught an AmazonServiceException, which means your request made it "
	                + "to Amazon S3, but was rejected with an error response for some reason.");
	        out.println("Error Message:    " + ase.getMessage());
	        out.println("HTTP Status Code: " + ase.getStatusCode());
	        out.println("AWS Error Code:   " + ase.getErrorCode());
	        out.println("Error Type:       " + ase.getErrorType());
	        out.println("Request ID:       " + ase.getRequestId());
	    } catch (AmazonClientException ace) {
	        out.println("Caught an AmazonClientException, which means the client encountered "
	                + "a serious internal problem while trying to communicate with S3, "
	                + "such as not being able to access the network.");
	        out.println("Error Message: " + ace.getMessage());
	    }
	    
		%>
		<p>Everything Deleted</p>
		<form name="input" action="/ece1779/man/managerUI.jsp" method="post">
			<input name="man_submit" type="submit" value="Return to managerUI" /><br/>
		</form>
	</body>
</html>