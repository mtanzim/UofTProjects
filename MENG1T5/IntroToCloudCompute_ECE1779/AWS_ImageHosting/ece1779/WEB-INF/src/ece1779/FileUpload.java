package ece1779;

import java.io.*;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.sql.DataSource;
import javax.swing.text.html.HTMLDocument.Iterator;

import org.apache.commons.fileupload.*;
import org.apache.commons.fileupload.disk.*;
import org.apache.commons.fileupload.servlet.*;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.List;
import java.util.UUID;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.auth.PropertiesCredentials;
import com.amazonaws.services.ec2.AmazonEC2;
import com.amazonaws.services.ec2.AmazonEC2Client;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.CannedAccessControlList;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.ListObjectsRequest;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.Bucket;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.ObjectListing;
import com.amazonaws.services.s3.model.S3ObjectSummary;

import org.im4java.core.*;


public class FileUpload extends HttpServlet {
    public void doPost(HttpServletRequest request, HttpServletResponse response)
	throws IOException, ServletException {
        try {
        	// Create a factory for disk-based file items
        	FileItemFactory factory = new DiskFileItemFactory();

        	// Create a new file upload handler
        	ServletFileUpload upload = new ServletFileUpload(factory);

        	// Parse the request
        	List /* FileItem */ items = upload.parseRequest(request);
        	            
            PrintWriter out = response.getWriter();
            response.setContentType("text/html");
            
            //check if user exists
            HttpSession session = request.getSession();
            String loggedIn =(String) session.getAttribute("loggedIn");  //("userID");
            //get logged in user
            String v_username=null;
            String v_id="empty";
            if (loggedIn != null) {
        		String[] string = loggedIn.split(" ");
        		v_username = string[0];
        		String v_password = string[1];
        		v_id = string[2];
        		v_username.replace("_", " ");
        		v_password.replace("_", " ");
            }
            else
            {
            	FileItem item1 = (FileItem)items.get(0);
            	if(item1.getSize()!=0)
            	{
            		String name = item1.getFieldName();
            		String value = item1.getString();
            		v_id=value;
            	}
            }
        	//Uploaded File
            FileItem theFile = (FileItem)items.get(1);
            if (theFile.getSize()!=0  && theFile.getName() !=null && !theFile.getName().isEmpty() && !v_id.equals("empty")) {				
					String userid = v_id;
	    			// filename on the client
		            String fileName = theFile.getName();
		   
		            // get root directory of web application
		            String path = "/tmp/";//this.getServletContext().getRealPath("/");
		            //four keys and four names
		            String key1 = "MyObjectKey_" + UUID.randomUUID();
		            String key2 = "MyObjectKey_" + UUID.randomUUID();
		            String key3 = "MyObjectKey_" + UUID.randomUUID();
		            String key4 = "MyObjectKey_" + UUID.randomUUID();
		            
		            String name1 = path+key1;
		            String name2 = path+key2;
		            String name3 = path+key3;
		            String name4 = path+key4;
		            
		            // store file in server
		            File file1 = new File(name1); 
		            theFile.write(file1);
		
		            // Use imagemagik to flip image
		            IMOperation op1 = new IMOperation();
		            op1.addImage();
		            op1.flip();
		            op1.addImage();
		            // Use imagemagik to black n white image
		            IMOperation op2 = new IMOperation();
		            op2.addImage();
		            op2.monochrome();
		            op2.addImage();
		
		            // Use imagemagik to thumbnail image
		            IMOperation op3 = new IMOperation();
		            op3.addImage();
		            op3.thumbnail(50);
		            op3.addImage();
		            
		            //cnversion time
		            ConvertCmd cmd = new ConvertCmd();
		            cmd.run(op1, name1,name2);
		            File file2 = new File(name2);
		            cmd.run(op2, name1,name3);
		            File file3 = new File(name3);
		            cmd.run(op3, name1,name4);
		            File file4 = new File(name4);
		
		            //PrintWriter out = response.getWriter();
		            s3SaveFile(file1, key1, out);
		        	s3SaveFile(file2, key2, out);
		        	s3SaveFile(file3, key3, out);
		        	s3SaveFile(file4, key4, out);
		            
		        	updateDatabase(userid, key1, key2, key3, key4, out);
		            
		            //response.setContentType("text/html");
		            
		            out.write("<html><head><title>Image Uploaded</title></head>");
		            out.write("<body>");
		            out.write("Image successfully uploaded! <br>");
		            out.write("<img src='https://s3.amazonaws.com/ece1779group10/" + key4 + "' />");
		            out.write("</body></html>");
		            out.write("<form name=\"input\" action=\"/ece1779/src/userUI.jsp\" method=\"post\">");
		            out.write("<input type=\"submit\" name = \"return\" value=\"Return to homepage\">");
		            out.write("</form>");
            }
            else if (v_id.equals("empty")) {
            	try {
    				response.sendRedirect(request.getContextPath() + "/src/userUI.jsp");
    			} catch (IOException e) {
    				// TODO Auto-generated catch block
    				e.printStackTrace();
    			}
            }
            else {        	
            	try {
            		String message = "IMFAIL";
            		request.getSession().setAttribute("ACK", message);
    				response.sendRedirect(request.getContextPath() + "/src/DisplayData.jsp");
    			} catch (IOException e) {
    				// TODO Auto-generated catch block
    				e.printStackTrace();
    			}
            }
    }
	catch (Exception ex) {
	    throw new ServletException (ex);
	}
	
}

public void updateDatabase(String userid, String key1, String key2, String key3, String key4, PrintWriter out) {
	Connection con = null;
	try{ 
        // In real life, you should get these values from a shopping cart
        //int userid = 1;
        
	    // Get DB connection from pool
	    DataSource dbcp = (DataSource)this.getServletContext().getAttribute("dbpool");
	    con = dbcp.getConnection();

	    // Execute SQL query
	    String query = "INSERT INTO images (userId, key1, key2, key3, key4) " + 
	    	"VALUES ('"+userid+"','"+key1+"','"+key2+"','"+key3+"','"+key4+"')";
		Statement stmt = con.createStatement();
		stmt.executeUpdate(query);
		stmt.close();
    }
	catch(Exception ex) {
          getServletContext().log(ex.getMessage());  
	}    	
	finally {
		try {
		con.close();
		} 
		catch (Exception e) {
            getServletContext().log(e.getMessage());  
		}
	}  
}   
     
    
public void s3SaveFile(File file, String key, PrintWriter out) throws IOException {

	BasicAWSCredentials awsCredentials = (BasicAWSCredentials)this.getServletContext().getAttribute("AWSCredentials");

	AmazonS3 s3 = new AmazonS3Client(awsCredentials);
    
    String bucketName = "ece1779group10";

        try {
            s3.putObject(new PutObjectRequest(bucketName, key, file));
            
            s3.setObjectAcl(bucketName, key, CannedAccessControlList.PublicRead);

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
}
}