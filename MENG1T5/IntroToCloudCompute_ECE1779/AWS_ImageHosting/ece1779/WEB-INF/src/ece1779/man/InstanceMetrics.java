package ece1779.man;

import java.io.IOException;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.sql.Timestamp;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;
import java.net.*;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.auth.PropertiesCredentials;
import com.amazonaws.services.cloudwatch.*;
import com.amazonaws.services.cloudwatch.model.*;
import com.amazonaws.services.ec2.AmazonEC2;
import com.amazonaws.services.ec2.AmazonEC2Client;
import com.amazonaws.services.ec2.model.Instance;
import com.amazonaws.services.ec2.model.Placement;
import com.amazonaws.services.ec2.model.Reservation;
import com.amazonaws.services.ec2.model.RunInstancesRequest;
import com.amazonaws.services.ec2.model.RunInstancesResult;
import com.amazonaws.services.ec2.model.StopInstancesRequest;
import com.amazonaws.services.ec2.model.StopInstancesResult;
import com.amazonaws.services.ec2.model.TerminateInstancesRequest;
import com.amazonaws.services.ec2.model.TerminateInstancesResult;
import com.amazonaws.services.elasticloadbalancing.AmazonElasticLoadBalancing;
import com.amazonaws.services.elasticloadbalancing.AmazonElasticLoadBalancingClient;
import com.amazonaws.services.elasticloadbalancing.model.DeregisterInstancesFromLoadBalancerRequest;
import com.amazonaws.services.elasticloadbalancing.model.DeregisterInstancesFromLoadBalancerResult;

import ece1779.man.reg_reg;


/*public class InstanceData {
	String d_systemid;
	String d_metric;
	Datapoints d_data;
	
}

public class Datapoints {
	String d_timestamp;
	Double d_average;
}*/


public class InstanceMetrics extends HttpServlet {
    public void doGet(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException
    {
    	
    	response.setIntHeader("Refresh",65);
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();
        out.println("<html>");
        out.println("<head>");
        out.println("<title>EC2 Instance Metrics</title>");
        out.println("</head>");
        out.println("<body>");
  

        
        //code for reading threshold
        //NEED ERROR CHECKING!!!!!!!!!!!!
        String path = this.getServletContext().getRealPath("/");        
        String t_file_name="threshold_info.txt";
        String threshold_file=path+t_file_name;
        //out.println("<br>"+read_threshold(out,threshold_file)+"<br>");
        String threshold_string=read_threshold(out,threshold_file);
        
        String tokens[]=threshold_string.split(",");
        
        double min_threshold=Double.parseDouble(tokens[0]);
        double max_threshold=Double.parseDouble(tokens[1]);
        double min_factor=Double.parseDouble(tokens[2]);
        double max_factor=Double.parseDouble(tokens[3]);
        
        
        //need to set an exception such that we don't kill the server
        int max_instance_num=20;
        int min_instance_num=1;
        //never kill the development instance
        String dev_instance="i-117a103f";
        
        
        //out.print("<br>"+min_threshold+"<br>");
        //out.print("<br>"+max_threshold+"<br>");
        //out.print("<br>"+min_factor+"<br>");
        //out.print("<br>"+max_factor+"<br>");

        
      //load threshold from file, set by the user
        //double max_threshold = 100;
        //double min_threshold =-5;
        
        //ami ID of the WORKER
        String aggregate_instance="ami-01f4cf68";
        String i_file_name="instance_info.txt";
        String instance_file=path+i_file_name;
        
        int decision =listInstances(out, max_threshold, min_threshold, aggregate_instance, instance_file);
        
        scale (out, decision, min_factor, max_factor, min_instance_num, max_instance_num, dev_instance, instance_file);
        
        out.print("<form name=\"input\" action=\"/ece1779/man/managerUI.jsp\" method=\"post\">");
        out.print("<input name=\"man_submit\" type=\"submit\" value=\"Stop Autoscaler\" /><br/>");
        out.print("</form>");
        
        out.println("</body>");
        out.println("</html>");
    }
    
    @SuppressWarnings("static-access")
	void scale (PrintWriter out, int decision, double min_factor, double max_factor,int min_instance_num,int max_instance_num, String dev_instance, String instance_file) throws IOException{
    	
    	
    	
    	BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
        AmazonEC2 ec2 = new AmazonEC2Client(awsCredentials);
    	
    	out.print("<br>SCALING POLICY<br>");
    	
    	//String path = this.getServletContext().getRealPath("/");        
        //String file_name="instance_info.txt";
        //String instance_file=path+file_name;
    	
        List <String> all_del_instances = new ArrayList<String>();
        
        
    	InputStream fis;
    	BufferedReader br;
    	String line;
    	
    	fis = new FileInputStream(instance_file);
    	br= new BufferedReader(new InputStreamReader(fis,Charset.forName("UTF-8")));
    	
    	String last_line="";
    	while( (line=br.readLine()) != null){
    		if (line.contains("ami-")){
    			//do nothing
    		}
    		else if (line.contains(dev_instance)) {
    			out.print("<br>Found Dev Instance!!!<br>");
    		}
    		else{
    			if (line.contains("i-")) {
    				all_del_instances.add(line.split(",")[0]);
    			}
    			
    		}
    		
    		last_line=line;
    	}
    	//del_instances.remove(del_instances.size()-1);
    	
    	out.print("<br><br>All Instances: "+all_del_instances.toString()+"<br>");
    	
    	br.close();
    	br=null;
    	fis=null;
    	
    	
    	//out.print("<br>"+last_line.trim()+"<br>");
    	
    	int num_inst = Integer.valueOf(last_line.trim()); 
    	
    	out.print("<br>Current instance count:"+num_inst+"<br>");
    	
    	//out.print("<br>Current instance count:"+num_inst+"<br>");
    	//out.print("<br>Current instance count:"+num_inst+"<br>");
    	
    	//hardcode
    	//decision=1;
    	
    	if (decision == 1){
    		out.print("<br>max factor:"+max_factor+"<br>");
    		//need to create new instances
    		int num_to_create = (int) Math.round((num_inst*max_factor)-num_inst);
    		out.print("<br>Creating "+num_to_create+" more<br>");
    		
    		int i=0;
    		
    		List <String> new_instances = new ArrayList<String>();
    		//new_instances.add("garbage");
    		
    		while (i<num_to_create){
    			
    			if ( (i+num_inst) < max_instance_num ) {
	    			out.print("<br><br>Warning: Threshold exceeded. Creating new instance.<br><br>");
	        		//load factor of expansion
	        		//find out the number of instances
	    			


	    			
	    			String new_id="";
	    			
	    			
	    		
	    			
	    			
	        		new_id = create_instance(out, ec2);
	 
	        		out.print("<br><br>New Instance: "+new_id+"<br>");
	        		new_instances.add(new_id);
    			}
    			else {
    				out.print("<br>Max Threshold exceeded<br>");
    			}
    			
        		i++;
        		
    		}
    		
    		out.print("<br><br>New Instances: "+new_instances.toString()+"<br>");
    		

    		

    		

    		
    	}
    	
    	
    	
    	
    	if (decision == -1){
    		out.print("<br>min factor:"+min_factor+"<br>");
    		//need to create new instances
    		int num_to_delete = (int) Math.round(num_inst - (num_inst*min_factor));
    		out.print("<br>Deleting "+num_to_delete+" Instances<br>");
    		
    		int i=0;
    		
    		List <String> del_instances = new ArrayList<String>();
    		
    		while (i<num_to_delete){
    			
    			if ( (num_inst-i) > min_instance_num ) {
	    			out.print("<br><br>Warning: Removing Instances.<br><br>");
	    			String temp_del="";
	    			temp_del=all_del_instances.get(i);
	        		del_instances.add(temp_del);
    			}
    			else {
    				out.print("<br>Min Threshold exceeded<br>");
    			}
    			
        		i++;
        		
    		}
    		
    		out.print("<br><br>DeletingInstances: "+del_instances.toString()+"<br>");
    		

            
    		
    		terminate_instance(out,del_instances, ec2);
    		
    		
    		//BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
    		AmazonElasticLoadBalancing elb = new AmazonElasticLoadBalancingClient(awsCredentials);
    		
    		reg_reg reg_entry = new reg_reg();
    		
    		
    		reg_entry.deregister_instance(out, del_instances, elb);
    		
    		//deregister instance list to load balancer
    		
    	}
    	
    	
    	
    	
    	//return line.trim();
    	
    	
    }
    
    
    String read_threshold (PrintWriter out, String file_name) throws IOException{
    	
    	InputStream fis;
    	BufferedReader br;
    	String line;
    	
    	fis = new FileInputStream(file_name);
    	br= new BufferedReader(new InputStreamReader(fis,Charset.forName("UTF-8")));
    	
    	line=br.readLine();
    	
    	br.close();
    	br=null;
    	fis=null;
    	//out.print("<br>"+line+"<br>");
    	
    	return line.trim();

    	
    }
    
    void write_threshold (PrintWriter out, int t_plus, int t_minus, double f_plus, double f_minus ) throws IOException{
    	//out.print("<br>CAME HERE!<br>");
    	
        String path = this.getServletContext().getRealPath("/");        
        String file_name="threshold_info.txt";
        
        // store file in server
        PrintWriter writer = new PrintWriter(path+file_name,"UTF-8");

        writer.println(t_plus+","+t_minus+","+f_plus+","+f_minus);
        writer.close();
    	
    }
    
    int listInstances(PrintWriter out, double max_t, double min_t, String aggregate_instance, String file_name) throws IOException {
    	
    	
    	BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
    	
        AmazonCloudWatch cw = new AmazonCloudWatchClient(awsCredentials);
        
        Dictionary <String, Dictionary> overall_dict = new Hashtable(); //holds all data
        
        
        
   
        try {

        	ListMetricsRequest listMetricsRequest = new ListMetricsRequest();
        	listMetricsRequest.setMetricName("CPUUtilization");
        	listMetricsRequest.setNamespace("AWS/EC2");
        	ListMetricsResult result = cw.listMetrics(listMetricsRequest);
        	java.util.List<Metric> 	metrics = result.getMetrics();
        	
        	int num_instances=0;
        	
        	for (Metric metric : metrics) {
        		String namespace = metric.getNamespace();
        		String metricName = metric.getMetricName();
        		List<Dimension> dimensions = metric.getDimensions();
            	GetMetricStatisticsRequest statisticsRequest = new GetMetricStatisticsRequest();
            	statisticsRequest.setNamespace(namespace);
            	statisticsRequest.setMetricName(metricName);
            	statisticsRequest.setDimensions(dimensions);
            	Date endTime = new Date();
            	Date startTime = new Date();
            	//startTime.setTime(endTime.getTime()-100000);
            	startTime.setTime(endTime.getTime()-120000);
            	statisticsRequest.setStartTime(startTime);
            	statisticsRequest.setEndTime(endTime);
            	statisticsRequest.setPeriod(60);
            	Vector<String>statistics = new Vector<String>();
            	
            	String value_type = "Average";
            	
            	statistics.add(value_type);
            	//statistics.add("Average");
            	statisticsRequest.setStatistics(statistics);
            	GetMetricStatisticsResult stats = cw.getMetricStatistics(statisticsRequest);
            	
            	out.print("<p>");
            	//out.print("Namespace = " + namespace + " Metric = " + metricName + " Dimensions = " + dimensions + "<br>");
            	//out.print("Values = " + stats.toString() + "<br><br>");
            	
            	String values = stats.toString();
            	
            	//String[] tokens ="";
     
            	String instanceID="Blank";
            	if (dimensions.toString().contains(",")){
            		String[] tokens =  dimensions.toString().split(",");
                	tokens =  tokens[1].split(":");
                	instanceID = tokens [1].trim(); //get instance ID
                	
                	//out.print(instanceID + "<br>"); //print instance ID
            	}

            	
            	String tokens[] = values.split("Timestamp:"); //split multiple timestamp instances

            	
            	//int timestamp_inst=0;
            	
            	Dictionary <String, Double> instance_dict = new Hashtable(); //only holds data for an instance
            	//tokens = values.split(", ");
            	
            	
            	//algorithm to only get the latest timestamp
            	
    			Date cur_date = new Date();
    			Date max_date = new Date();
    			max_date.setTime(cur_date.getTime()-1200000);
    			double max_value=-1;
    			
            	
            	for (int i=0; i < tokens.length; i++){
            		if (i != 0) {
      
            			
            			String [] inner_token = tokens[i].split(",");
            			String temp_time=inner_token[0].replace("{Timestamp:","").trim();
            			Double temp_value=Double.valueOf(inner_token[1].replace(value_type+":","").trim());
            			
            			//Date max_timestamp = new Date();
            			//Date cur_timestamp = new Date();
            			
            			//out.print("<br>"+temp_time+"<br>");
            			SimpleDateFormat dateFormat = new SimpleDateFormat("EEE MMM dd HH:mm:ss z yyyy");
            			
            			
            			
						try {
							cur_date = dateFormat.parse(temp_time);
							
							//out.print("<br>Parsed: "+cur_date.toString()+"<br>");
							
							if (cur_date.after(max_date)){
								max_date=cur_date;
								max_value=temp_value;
								//out.print("New max!");
							}
						
							
						} catch (ParseException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
            			
            			
            			//Timestamp cur_ts = new Timestamp();
            			
            			
            			
            			
            			//timestamp_inst ++;
            			
            			
            		}
            	}
            	
            	if(max_value>-1){
            		instance_dict.put(max_date.toString(), max_value);
            		overall_dict.put(instanceID, instance_dict);
            	}
            	
    			
            	
            	
            	
        		
            }
        	
        	//check the status of the AMI
        	int decide=0;
        	boolean check_now=false;
        	
        	Enumeration <String> enumKey = overall_dict.keys();
        	String ami_info="";

        	// get root directory of web application
            String path = this.getServletContext().getRealPath("/");        

            //String file_name="instace_info.txt";
            
            // store file in server
            PrintWriter writer = new PrintWriter(file_name, "UTF-8");

            writer.println("id,timestamp,CPUUtilization");
        	
        	
        	
        	while (enumKey.hasMoreElements()){
        		String key = enumKey.nextElement();
        		
        		
        		
        		//only use image aggreagate information for decision
        		if (key.equals(aggregate_instance)){
        			//out.print("<br>AMI FOUND<br>");
        			check_now=true;
        			ami_info+=key+",";
        		}
        		
        		//out.print(instanceID);
        		
        		//only print instance info, not aggregate
            	if (key.matches("i-(.*)")){
            		out.print("<br><br>" + key);
            		writer.print(key+",");
            		//out.print("caught instance");
            		
            		List <String> new_instances = new ArrayList<String>();
            		new_instances.add(key);
            	
            	        	
    	        	String myid=null;
    				try {
    					
    					myid = retrieveInstanceId();
    					new_instances.remove(myid);
    					out.print ("<br>Don't register self: "+myid+"<br>");
    					
    				} catch (Exception e) {
    					// TODO Auto-generated catch block
    					e.printStackTrace();
    				}
        
            		 
            				
    				out.println("<br>"+new_instances.toString()+"<br>");
            		
            		
            		
            	
            		//try to register instance if not registered
            		//BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
            		AmazonElasticLoadBalancing elb = new AmazonElasticLoadBalancingClient(awsCredentials);
            		
            		reg_reg reg_entry = new reg_reg();
            		
            		
            		reg_entry.register_instance(out, new_instances, elb);
            		
            		
            		
            		num_instances ++;
            	}
        		
        		Enumeration <String> enumKey2 = overall_dict.get(key).keys();
        		
        		
        		
        		
        		while (enumKey2.hasMoreElements()){
        			String key2 = enumKey2.nextElement();
        			
        			
        			
        			if (key.matches("i-(.*)")){
        			
        				out.print("<br>" + key2);
        				writer.print(key2+",");
        			}
            		
            		Double temp_value= (Double) overall_dict.get(key).get(key2);
            		
					if (check_now){
						//there is only 1 ami
						check_now=false;
						ami_info+=key2+","+temp_value;
						
						if (temp_value > max_t){
		    				//is_warning=true;
		    				decide=1;
		    				//check_now=false;
		    				
		    			}
		    			else if (temp_value < min_t){
		    				decide=-1;
		    				//check_now=false;
		    			}
					        				
					}
					
					
            		
					if (key.matches("i-(.*)")){
						writer.println(temp_value);
						out.print("&nbsp&nbsp&nbsp" + temp_value);
					}
					

        		}
        		
        		
        		
        	}
        	writer.println(ami_info);
        	writer.println(num_instances);
        	writer.close();
        	
        	
        	
        	out.println("<br><br>Number of instances: "+ num_instances+"<br>");
        	out.println("</p>");
        	return decide;
        	
        	
        	//FUNCTIOM ENDS HERE

        	
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
		return 0;
    }
    

    public static String retrieveInstanceId() throws Exception
    {
    	String EC2Id = "";
    	String inputLine;
    	URL EC2MetaData = new URL("http://169.254.169.254/latest/meta-data/instance-id");
    	URLConnection EC2MD = EC2MetaData.openConnection();
    	BufferedReader in = new BufferedReader(
    	new InputStreamReader(
    	EC2MD.getInputStream()));
    	while ((inputLine = in.readLine()) != null)
    	{
    		EC2Id = inputLine;
    	}
    	in.close();
    	return EC2Id;
    } 
    
    
    public void terminate_instance(PrintWriter out, List <String> sys_id, AmazonEC2 ec2) throws IOException {
    	
        
    	/*
         * Important: Be sure to fill in your AWS access credentials in the
         *            AwsCredentials.properties file before you try to run this
         *            sample.
         * http://aws.amazon.com/security-credentials
         */
    	
    	
    	//ec2.describeInstances();
        
        out.println("===========================================");
        out.println("Terminating with Amazon EC2");
        out.println("===========================================\n");

        out.println("<br>"+sys_id.toString()+"<br>");
        
        try {
        	//String imageId = "ami-01f4cf68";
        	
        	String myid=null;
			try {
				
				myid = retrieveInstanceId();
				sys_id.remove(myid);
				out.print ("<br>Don't remove self: "+myid+"<br>");
				
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			out.println("<br>"+sys_id.toString()+"<br>");
        	
        	TerminateInstancesRequest request = new TerminateInstancesRequest(sys_id);        	
        	TerminateInstancesResult result = ec2.terminateInstances(request);

        	
        	
        	out.println("Request Info = " + request.toString());
        	out.println("Result Info = " + result.toString());

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
    
    
   
/*
    
    void stop_instance(PrintWriter out, List <String> sys_id) throws IOException {

    	
    	BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
    	
		
    	
        AmazonEC2 ec2 = new AmazonEC2Client(awsCredentials);
        //AmazonElasticLoadBalancing elb = new AmazonElasticLoadBalancingClient(awsCredentials);
        
        
        
        out.println("===========================================");
        out.println("Stopping with Amazon EC2");
        out.println("===========================================\n");

        try {
        	//String imageId = "ami-01f4cf68";
        	StopInstancesRequest request = new StopInstancesRequest(sys_id);
        	//request.setKeyName("mokammel_3");
        	StopInstancesResult result = ec2.stopInstances(request);
        	//Reservation reservation = result.getReservation();
        	//List<Instance> instances = reservation.getInstances();
        	//Instance inst = instances.get(0);

        	
        	out.println("Request Info = " + request.toString());
        	out.println("Result Info = " + result.toString());

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
    	
*/ 
    ///create instance
    
    
    public String create_instance(PrintWriter out, AmazonEC2 ec2) throws IOException {
        /*
         * Important: Be sure to fill in your AWS access credentials in the
         *            AwsCredentials.properties file before you try to run this
         *            sample.
         * http://aws.amazon.com/security-credentials
         */
    	
    	
    	//BasicAWSCredentials awsCredentials = (BasicAWSCredentials)getServletContext().getAttribute("AWSCredentials");
        //AmazonEC2 ec2 = new AmazonEC2Client(awsCredentials);
        
        
        out.println("===========================================");
        out.println("Getting Started with Amazon EC2");
        out.println("===========================================\n");

        try {
        	String imageId = "ami-01f4cf68";
        	String key_name="ece1779";
        	//Collection <String> sec_grp = new Collection;
        	
        	String sec_grp="mokammel_ece1779";
        	String av_zone="us-east-1d";
        	Boolean monitoring=true;
        	
        	
        	Collection <String> sec_grp_col = Arrays.asList(sec_grp);
        	Placement region = new Placement(av_zone) ;
        	
        	//region.setAvailabilityZone(av_zone);

        	RunInstancesRequest request = new RunInstancesRequest(imageId,1,1);
        	request.setKeyName(key_name);
        	request.setSecurityGroups(sec_grp_col);
        	request.setPlacement(region);
        	request.setMonitoring(monitoring);
        	RunInstancesResult result = ec2.runInstances(request);
        	Reservation reservation = result.getReservation();
        	List<Instance> instances = reservation.getInstances();
        	Instance inst = instances.get(0);

        	
        	
        	out.println("<br>Instance Info = " + inst.toString());
        	
        	String [] tokens = inst.toString().split(",");
        	tokens = tokens[0].split("InstanceId:");
        	String id_name=tokens[1];
        	
        	return id_name;
			//String temp_time=inner_token[0].replace("{Timestamp:","").trim();
        	
        	//return instance ID, so that it can be registered
        	

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
        return "";
    }
    
    ///end create instance
    
    

    

    
}