package ece1779.man;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.services.elasticloadbalancing.AmazonElasticLoadBalancing;
import com.amazonaws.services.elasticloadbalancing.AmazonElasticLoadBalancingClient;
import com.amazonaws.services.elasticloadbalancing.model.*;

public class reg_reg extends HttpServlet {
//public class reg_reg{
	/*
	
    public void doGet(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException
    {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();
        out.println("<html>");
        out.println("<head>");
        out.println("<title>Elastic Balancing</title>");
        out.println("</head>");
        out.println("<body>");
  
        
        
    	//dereg/reg devices to ELB
    	boolean dereg=false;
    	String to_dereg_single="i-807e48ae";
    	
    	
    	List <String> to_dereg = Arrays.asList(to_dereg_single);
    	
    	
    	
    	if (dereg){

    		deregister_instance (out, to_dereg);

    		
    	}
    	else{
    		register_instance (out, to_dereg);
    	}

        //cloudWatchExample(out);
        
        out.println("</body>");
        out.println("</html>");
    }
    
    */
    public void do_jack(PrintWriter out, List <String> sys_id){
    	out.print("<br>BIIIIIITCH");
    	out.print("<br>"+sys_id.toString());
    }
    
    public void register_instance(PrintWriter out, List <String> sys_id, AmazonElasticLoadBalancing elb) throws IOException {

    	
    	
    	//BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
        //AmazonElasticLoadBalancing elb = new AmazonElasticLoadBalancingClient(awsCredentials);
        
        
        
        out.println("===========================================");
        out.println("Register with Amazon ELB");
        out.println("===========================================\n");

        try {
        	
        	String elb_name= "testlb";
        	
        	List<Instance> instances = new ArrayList<Instance>();
        	
        	
        	//if (sys_id != null){
	        	for (int i=0; i<sys_id.size();i++) {
	        		//String temp = sys_id.get(i);
	        		Instance cur_ins = new Instance();
	        		cur_ins.setInstanceId(sys_id.get(i));
	        		instances.add(cur_ins);
	        		
	        	}
        	//}
        	
        	
        	
        	
        	
        	RegisterInstancesWithLoadBalancerRequest request = new RegisterInstancesWithLoadBalancerRequest(elb_name,instances);
        	RegisterInstancesWithLoadBalancerResult result = elb.registerInstancesWithLoadBalancer(request);
        	//DeregisterInstancesFromLoadBalancerResult result = new DeregisterInstancesFromLoadBalancerResult();
        	
        	out.println("Request Info = " + request.toString());
        	out.println("Result Info = " + result.getInstances());

                    } catch (AmazonServiceException ase) {
            out.println("Caught an AmazonServiceException, which means your request made it "
                    + "to Amazon EC2, but was rejected with an error response for some reason.");
            out.println("Error Message:    " + ase.getMessage());
            out.println("HTTP Status Code: " + ase.getStatusCode());
            out.println("AWS Error Code:   " + ase.getErrorCode());
            out.println("Error Type:       " + ase.getErrorType());
            out.println("Request ID:       " + ase.getRequestId());
        } catch (AmazonClientException ace) {
            out.println("Caught an AmazonClientException, which means the client encountered "
                    + "a serious internal problem while trying to communicate with EC2, "
                    + "such as not being able to access the network.");
            out.println("Error Message: " + ace.getMessage());
        }
    }
    
    
    
    public void deregister_instance(PrintWriter out, List <String> sys_id, AmazonElasticLoadBalancing elb) throws IOException {

    	
    	
    	//BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
        //AmazonElasticLoadBalancing elb = new AmazonElasticLoadBalancingClient(awsCredentials);
        
        
        
        out.println("===========================================");
        out.println("Deregister with Amazon ELB");
        out.println("===========================================\n");

        try {
        	
        	String elb_name= "testlb";
        	
        	List<Instance> instances = new ArrayList<Instance>();
        	
        	
        	for (int i=0; i<sys_id.size();i++) {
        		//String temp = sys_id.get(i);
        		Instance cur_ins = new Instance();
        		cur_ins.setInstanceId(sys_id.get(i));
        		instances.add(cur_ins);
        		
        	}
        	
        	
        	
        	DeregisterInstancesFromLoadBalancerRequest request = new DeregisterInstancesFromLoadBalancerRequest(elb_name,instances);
        	DeregisterInstancesFromLoadBalancerResult result = elb.deregisterInstancesFromLoadBalancer(request);
        	//DeregisterInstancesFromLoadBalancerResult result = new DeregisterInstancesFromLoadBalancerResult();
        	
        	out.println("Request Info = " + request.toString());
        	out.println("Result Info = " + result.getInstances());

                    } catch (AmazonServiceException ase) {
            out.println("Caught an AmazonServiceException, which means your request made it "
                    + "to Amazon EC2, but was rejected with an error response for some reason.");
            out.println("Error Message:    " + ase.getMessage());
            out.println("HTTP Status Code: " + ase.getStatusCode());
            out.println("AWS Error Code:   " + ase.getErrorCode());
            out.println("Error Type:       " + ase.getErrorType());
            out.println("Request ID:       " + ase.getRequestId());
        } catch (AmazonClientException ace) {
            out.println("Caught an AmazonClientException, which means the client encountered "
                    + "a serious internal problem while trying to communicate with EC2, "
                    + "such as not being able to access the network.");
            out.println("Error Message: " + ace.getMessage());
        }
    }
    
}
