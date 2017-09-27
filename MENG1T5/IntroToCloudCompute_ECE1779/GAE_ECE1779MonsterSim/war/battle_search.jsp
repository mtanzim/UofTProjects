<%@ page import="ece1779.monstersim.monster" %>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="java.util.List" %>
<%@ page import="com.google.appengine.api.users.User" %>
<%@ page import="com.google.appengine.api.users.UserService" %>
<%@ page import="com.google.appengine.api.users.UserServiceFactory" %>
<%@ page import="com.google.appengine.api.datastore.DatastoreServiceFactory" %>
<%@ page import="com.google.appengine.api.datastore.DatastoreService" %>
<%@ page import="com.google.appengine.api.datastore.Query" %>
<%@ page import="com.google.appengine.api.datastore.Entity" %>
<%@ page import="com.google.appengine.api.datastore.FetchOptions" %>
<%@ page import="com.google.appengine.api.datastore.Key" %>
<%@ page import="com.google.appengine.api.datastore.KeyFactory" %>
<%@ page import="java.text.NumberFormat" %>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<%@ page import="com.google.appengine.api.datastore.PreparedQuery" %>
<%@ page import="com.google.appengine.api.datastore.Query.Filter" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterPredicate" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterOperator" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilter" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilterOperator" %>

<%@ page import="com.google.appengine.api.memcache.MemcacheService" %>
<%@ page import="com.google.appengine.api.memcache.MemcacheServiceException" %>
<%@ page import="com.google.appengine.api.memcache.MemcacheServiceFactory" %>

<%@ page import="java.util.ArrayList" %>
<%@ page import="java.awt.event.ActionListener" %>
<%@ page import="java.awt.event.ActionEvent" %>


<html>

	<head>
    	<link type="text/css" rel="stylesheet" href="/stylesheets/main.css" />
    	<script src="//code.jquery.com/jquery-1.9.1.js"></script>
    	<script type="text/javascript">
    	function myFunction(my_pet_name){
			var form = document.createElement("form");
			form.name = "input";
			form.method = "post";
			form.action = "battle_start.jsp";
			var user_pet_name = document.createElement("input");
			var opp_pet_name = document.createElement("input");
			var whoattack = document.createElement("input");
					
			whoattack.type="text";
			whoattack.name="whoattack";
			whoattack.value="nobody";
			
			opp_pet_name.type = "text";
			opp_pet_name.name = "opponent";
			opp_pet_name.value = $('input:radio[name=UserSelected]:checked').val();
			
			user_pet_name.type="text";
			user_pet_name.name="user";
			user_pet_name.value=my_pet_name;
			
		    form.appendChild(whoattack);
		    form.appendChild(opp_pet_name);
		    form.appendChild(user_pet_name);
		    form.submit(); 
    	}
    	</script>
    	<style>
    	form {
		    display: inline;
		}
    	html { 
			background: url(battle_background.png) no-repeat center center fixed; 
			-webkit-background-size: cover; 
			-moz-background-size: cover; 
			-o-background-size: cover; 
			background-size: cover; 
		}  	
		#customers
		{
			font-family:"Trebuchet MS", Arial, Helvetica, sans-serif;
			width:100%;
			border-collapse:collapse;
		}
		
		#customers .ui-selecting
		{
			background: #FECA40;
		}
		
 	 	#customers .ui-selected
		{
			background: #F39814; color: white;
		}
		
		#customers td, #customers th 
		{
			font-size:1em;
			text-align:center;
			border:1px solid #98bf21;
			padding:3px 7px 2px 7px;
			background-color:#ffffff;
		}
		#customers th 
		{
			font-size:1.1em;
			text-align:center;
			padding-top:5px;
			padding-bottom:4px;
			background-color:#A7C942;
			color:#ffffff;
		}
		#customers tr.alt td 
		{
			color:#000000;
			text-align:center;
			background-color:#EAF2D3;
		}
    	</style>
  	</head>

  	<body>
  	
  	<h1> Pre-Battle! </h1>
  	<p> Selected Pet </p>
  	
  			<%!
				//INITIALIZE PET VARIABLES
		  		public  Integer MAX_NUM_PETS=25;
		  	
		  	
		  		//all of the below variables will come from the database
		  		//String guestbookName = request.getParameter("guestbookName");
		  		//String avatar_url="http://images.neopets.com/images/nf/giantmeepitofpeace_notdoom.gif";
		  		public  String avatar_url="";
		  		public  String pet_name = "";
		  		
		  		public  String pet_owner ="";
		  		
		  		public  Integer pet_level = 0;
				public  Integer max_exp=0;
				public  Integer cur_exp=0;
				  		
				public  Integer cur_health=0;
				public  Integer max_health=0;
				public  Integer defense_pt = 0;
				public  Integer attack_pt = 0;
				public  Integer speed_pt = 0;
				public  Integer user_pet_speed=0;
				  		
				public  Integer bat_won=0;
				public  Integer bat_lost=0;
				  		//Double win_rat = (double)bat_won/(double)(bat_lost+bat_won);
		  		public  Double win_rat=0.0;	
  				
  	
  			%>
  			
  			<%!
		 	public void set_pet_variables (Entity pet_stat) {
		  		
		  		avatar_url=pet_stat.getProperty("avatar_url").toString();
		  	     
					
		  		if (pet_stat.getProperty("pet_name").toString()==null) {
		  			pet_name="no name";	
		  		}
		  		else {
		  			pet_name = pet_stat.getProperty("pet_name").toString();
		  		}
		  		
		  		if (pet_stat.getProperty("user").toString()==null) {
		  			pet_owner="no owner";	
		  		}
		  		else {
		  			pet_owner = pet_stat.getProperty("user").toString();
		  		}
		  		pet_level = Integer.parseInt(pet_stat.getProperty("pet_level").toString());
		  		max_exp=Integer.parseInt(pet_stat.getProperty("max_exp").toString());
		  		cur_exp=Integer.parseInt(pet_stat.getProperty("cur_exp").toString());
		  		
		  		cur_health=Integer.parseInt(pet_stat.getProperty("cur_health").toString());
		  		max_health=Integer.parseInt(pet_stat.getProperty("max_health").toString());
		  		defense_pt = Integer.parseInt(pet_stat.getProperty("defense_pt").toString());
		  		attack_pt = Integer.parseInt(pet_stat.getProperty("attack_pt").toString());
		  		speed_pt = Integer.parseInt(pet_stat.getProperty("speed_pt").toString());
		  		
		  		bat_won=Integer.parseInt(pet_stat.getProperty("bat_won").toString());
		  		bat_lost=Integer.parseInt(pet_stat.getProperty("bat_lost").toString());
		  		
		  		if ((bat_won+bat_lost)==0){
		  			win_rat=0.0;
		  		}
		  		else {
		  			win_rat = (double)bat_won/(double)(bat_lost+bat_won);
		  		}
		  	}
  			
  			%>
  			<%!//Function to see if the user's pet is "you" (pet_1) or "them" (pet_2)
			public String whoAreYou (String user_pet_name)
			{
  				String pet1 = "";
  				String pet2 = "";
  				DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
  			    Filter pet_1_filter = new FilterPredicate ("pet_1", FilterOperator.EQUAL, user_pet_name);
  			    Filter pet_2_filter = new FilterPredicate ("pet_2", FilterOperator.EQUAL, user_pet_name);
  			    Filter combined_filter = CompositeFilterOperator.or(pet_1_filter, pet_2_filter);
  			    Query q_b = new Query("pet_battle");
  			    q_b.setFilter(combined_filter);
  				PreparedQuery pq_b = petstatstore.prepare(q_b);
  			    List<Entity> pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(1));
  				if (pet_stats.isEmpty())
  				{
  					return "";
  		    	}
  		    	else 
  		    	{
  		   			for (Entity pet_stat: pet_stats) 
  		   			{
  		   				pet1 = pet_stat.getProperty("pet_1").toString();
  		   				pet2 = pet_stat.getProperty("pet_2").toString();
  		   				if (pet1.equals(user_pet_name))
  		   					return "you";
  		   				else if (pet2.equals(user_pet_name))
  		   					return "them";
  		   				else
  		   					return "";
					}
		    	}
				return "";
			}
  			%>
  			<%!
  			public String whoAttacking (String user_pet_name)
			{
  				String pet1 = "";
  				String pet2 = "";
  				DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
  			    Filter pet_1_filter = new FilterPredicate ("pet_1", FilterOperator.EQUAL, user_pet_name);
  			    Filter pet_2_filter = new FilterPredicate ("pet_2", FilterOperator.EQUAL, user_pet_name);
  			    Filter combined_filter = CompositeFilterOperator.or(pet_1_filter, pet_2_filter);
  			    Query q_b = new Query("pet_battle");
  			    q_b.setFilter(combined_filter);
  				PreparedQuery pq_b = petstatstore.prepare(q_b);
  			    List<Entity> pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(1));
  				if (pet_stats.isEmpty())
  				{
  					return "";
  		    	}
  		    	else 
  		    	{
  		   			for (Entity pet_stat: pet_stats) 
  		   			{
  		   				return pet_stat.getProperty("nextattacker").toString();
  		   					
					}
		    	}
				return "";
			}
			%>
			<%!//Function to send health to DB
				public void cleanup (String user_pet_name)
				{
					DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
	  			    Filter pet_1_filter = new FilterPredicate ("pet_1", FilterOperator.EQUAL, user_pet_name);
	  			    Filter pet_2_filter = new FilterPredicate ("pet_2", FilterOperator.EQUAL, user_pet_name);
	  			  	Filter done_filter = new FilterPredicate ("nextattacker", FilterOperator.EQUAL, "done");
	  			    Filter combined_filter1 = CompositeFilterOperator.or(pet_1_filter, pet_2_filter);
	  			  	Filter combined_filter = CompositeFilterOperator.and(combined_filter1, done_filter);
	  			    Query q_b = new Query("pet_battle");
	  			    q_b.setFilter(combined_filter);
	  				PreparedQuery pq_b = petstatstore.prepare(q_b);
	  			    List<Entity> pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(1));
	  				if (pet_stats.isEmpty())
	  				{
	  		    	}
	  		    	else
		  		    {
			   			for (Entity pet_stat: pet_stats) 
			   			{
			   					petstatstore.delete(pet_stat.getKey()); 
						}
			    	}
				}
			%>
			
  			<%
  			
  			UserService userService = UserServiceFactory.getUserService();
  	 		User user = userService.getCurrentUser();	
  	 		String user_name=user.getNickname();
  	 		
			NumberFormat nf = NumberFormat.getInstance();
			nf.setMaximumFractionDigits(2);
			nf.setMinimumFractionDigits(2);
			
			//softcode pet_name 
			String user_pet_name = request.getParameter("pet_name");
			String op_name = request.getParameter("opp_pet_name");
			String search_type =request.getParameter("search_type");
		    DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
		    
		    boolean in_battle=false;
		    

		    Filter pet_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, user_pet_name);
		    Query q = new Query("pet_stats");
		    q.setFilter(pet_only);
		    PreparedQuery pq = petstatstore.prepare(q);
		    List<Entity> pet_stats = pq.asList(FetchOptions.Builder.withLimit(MAX_NUM_PETS)); 

			if (pet_stats.isEmpty()){
			
		    	%>
		        <p><font face="verdana">No Pets!</p>
		        
		        <form action="create.jsp" method="post">
					<h2> <input name="create" type="submit" value="Create New Monster!" /></h2>
				</form>
			
				<%
		    }
		    else {
		    	boolean first = true;
		   		for (Entity pet_stat_root : pet_stats) {
		   			set_pet_variables (pet_stat_root);
					String opp_name = pet_stat_root.getProperty("opp_name").toString();
					String whoattack = "nobody";
					int ishealthzero = Integer.parseInt(pet_stat_root.getProperty("cur_health").toString());
					cleanup(user_pet_name);//cleanup all battles that are in "done" state
					String whoareyou = "";
					//pet is already in battle, provide option to continue
		   			if (!opp_name.isEmpty()){
		   				in_battle=true;
		   				whoattack = whoAttacking(user_pet_name);//can be request, you, them (done is all cleaned up)
		   				whoareyou = whoAreYou(user_pet_name);//you initiates battle request, them responds
		   				//need to find out who is attacking next
		   				%>
		          		<!--variable "pet_name" posted from the previous page-->
		          		<h1><font face="verdana"><%=pet_name%></h1>
		         		<p><font face="verdana">In Battle with <%=opp_name%></p>
<%-- 		         		next attacker is <%=whoattack %><br> --%>
		         		<%
		         		if (whoattack.equals("request"))
		         		{//someone recieved a request
		         			if (whoareyou.equals("them"))
		         			{
		         				%>
				         		<form action="battle_start.jsp" method="post">
			        				<h2> 
			        				<input readonly name="user" type="hidden" value="<%=pet_name %>">
			        				<input readonly name="opponent" type="hidden" value="<%=opp_name %>">
			        				<input name="whoattack" type="hidden" value="accept" />
			        				<input name="nextattacker" type="hidden" value=<%=whoattack %> />
			        				<input name="battle" type="submit" value="Accept Battle!" />
			        				</h2>
			       				</form>
			       				<form action="battle_start.jsp" method="post">
			        				<h2> 
			        				<input readonly name="user" type="hidden" value="<%=pet_name %>">
			        				<input readonly name="opponent" type="hidden" value="<%=opp_name %>">
			        				<input name="whoattack" type="hidden" value="reject" />
			        				<input name="nextattacker" type="hidden" value=<%=whoattack %> />
			        				<input name="battle" type="submit" value="Reject Battle!" />
			        				</h2>
			       				</form>
				         		<%
		         			}
		         			else if (whoareyou.equals("you"))
		         			{
		         				%>
			       				<form action="battle_start.jsp" method="post">
			        				<h2> 
			        				<input readonly name="user" type="hidden" value="<%=pet_name %>">
			        				<input readonly name="opponent" type="hidden" value="<%=opp_name %>">
			        				<input name="whoattack" type="hidden" value="reject" />
			        				<input name="nextattacker" type="hidden" value=<%=whoattack %> />
			        				<input name="battle" type="submit" value="Cancel Request!" />
			        				</h2>
			       				</form>
				         		<%
		         			}
		         		}
		         		else
		         		{
		         		%>
		         		<form action="battle_start.jsp" method="post">
	        				<h2> 
	        				<input readonly name="user" type="hidden" value="<%=pet_name %>">
	        				<input readonly name="opponent" type="hidden" value="<%=opp_name %>">
	        				<input name="whoattack" type="hidden" value=<%=whoattack %> />
	        				<input name="nextattacker" type="hidden" value=<%=whoattack %> />
	        				<input name="battle" type="submit" value="Continue Battle!" />
	        				</h2>
	       				</form>
		         		<%
                        }
                    }
		   			else if (ishealthzero == 0)
		   			{
		   				in_battle=true;
		   				%>
		   				<h1><font face="verdana"><%=pet_name%></h1>
				        <p><font face="verdana">FEED YOUR PET!</p>
						<%
		   				
		   			}
		         	 else {
		   				if (first) {
		   					first = false;
		   					%>
		   			        <table id = "customers" border="1" style="width:300px">
				       		<tr id = "head">
								<th>Pet Name</th>
								<th>Image</th>
								<th>Owner</th>
								<th>Level</th>
								<th>Experience</th>
								<th>HP</th>
								<th>Attack</th>
								<th>Defense</th>
								<th>Speed</th>
								
								<th>Battle Won</th>
								<th>Battle Lost</th>
								<th>Win Ratio</th>
							</tr>
							<%
		   				} 
			   			//WHY IS THIS HERE? need to save the speed of user's pet to pass to batle_start
						user_pet_speed = speed_pt;
						String row_name = pet_owner+pet_name;
		          	    %>
		          	    	<tr id="<%=row_name%>">
			         			<td scope="col"><%=pet_name%></td>
			         			<td> <img alt="" src="icons/<%= avatar_url%>.gif";" /></td>
			         			<td><%=pet_owner%></td>
			         			<td><%=pet_level%></td>		
			         			<td><%=cur_exp%>/<%= max_exp%> </td>
			         			<td><%=cur_health%>/<%= max_health%></td>
			         			<td><%=attack_pt%></td>
			         			<td><%=defense_pt%></td>
			         			<td><%=speed_pt%></td>
			         			<td><%=bat_won%></td>
			         			<td><%=bat_lost%> </td>
			         			<td><%=nf.format(win_rat.doubleValue())%> </td>
						</tr>
		          		<% 
		   			
		   			} //else
		   		} //for
				%> </table><% 
		    } //bigger else
			
			if (!in_battle) {
			
				//use radio buttons to either search by user, or pet name
				
	     		%>
	     		<h2><font face="verdana">Potential Opponents</h2>
	       	
	       		<form name="input" action="battle_search.jsp" method="post">
	       			<input type="radio" name="search_type" value="user">User</input>
					<input type="radio" name="search_type" value="pet" checked="checked">Pet</input>
					Search: <input type="text" name="opp_pet_name">
					<input type="submit" value="Submit">
					<input type="hidden" name="pet_name" value="<%=user_pet_name%>">
				</form>
	       		
	       		<% 
	       		     		
	       		//Display opponents -------------------------
	       		
	       		//DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
	       		
	       		Filter not_user_only = new FilterPredicate ("user", FilterOperator.NOT_EQUAL, user_name);
	       		Filter op_only = null;
	       		//Filter op_not_user = null;
	       		Filter combined_filter = null;
	       		//Filter op_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, op_name);
	       		//Filter op_not_user = CompositeFilterOperator.and(op_only, not_user_only);
	       		
	       		//op_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, op_name);
	   			//op_not_user = CompositeFilterOperator.and(op_only, not_user_only);
	       		
	   			boolean incache = false;
	   			Entity value = null;
	   			MemcacheService syncCache = MemcacheServiceFactory.getMemcacheService();
	       		if (!op_name.isEmpty()) {
	       			
	       			%>
	           		<!--variable "pet_name" posted from the previous page-->
	    	        <h3><font face="verdana">Searched <%=search_type%>: <%=op_name%></h3>
	    			<%
	    			
	    			//sets a different filter based on the selected radio button
	    			if (search_type.contentEquals("user")) {
	    				
	    				if (op_name.contentEquals(user_name)){
	    					%>
	    	    	        <p><font face="verdana">Can't battle yourself!</p>
	    	    			<%
	    	    			combined_filter = not_user_only;
	    					
	    				}
	    				else {
	    				    op_only = new FilterPredicate ("user", FilterOperator.EQUAL, op_name);
	    					combined_filter=op_only;
	    				}
	    			}
	    			else if (search_type.contentEquals("pet")) {
    					if (!syncCache.contains(op_name)) {
    						incache = false;
    						op_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, op_name);
	    					combined_filter= CompositeFilterOperator.and(op_only, not_user_only);
    					}
    					else {
    						value = (Entity) syncCache.get(op_name);
    						incache = true;
    					}
	    			}
	    			else{
	    				combined_filter = not_user_only;
	    			}
	       		} //if (!op_name.isEmpty())
	       		else {
			    	combined_filter = not_user_only;
	       		}

	       		Query q_b = new Query("pet_stats");
       			List<Entity> op_stats = new ArrayList<Entity>();
	       		if(incache) {
	       			op_stats.add(value);
	       			incache = false;
	       		}
	       		else {
				    q_b.setFilter(combined_filter);
				    //add filter to not display user's own pets
				    PreparedQuery pq_b = petstatstore.prepare(q_b);
				    op_stats = pq_b.asList(FetchOptions.Builder.withLimit(MAX_NUM_PETS)); 
				    //List <String> pet_owners = new ArrayList<String>();
				    if (op_stats.isEmpty()){
				    	%>
				        <p><font face="verdana">No opponents found!</p>
						<%
				    }
				    else {
					    for (Entity op_stat_root : op_stats) {
							syncCache.put(op_stat_root.getProperty("pet_name").toString(), op_stat_root);
					    }
				    }
	       		}
	       		
				if (op_stats.isEmpty()){
			    	%>
			        <p><font face="verdana">No opponents found!</p>
					<%
			    }
			    else {
			    	
			    	boolean first = true;
			    	boolean second = false;
			   		for (Entity op_stat_root : op_stats) {
			   			
			   			set_pet_variables (op_stat_root);
			   			String opp_name = op_stat_root.getProperty("opp_name").toString();
			   			if (opp_name.isEmpty()) {
				   			//pet_owners.add(pet_stat.getKey().toString());
			         	     if (first) {
			         	    	 first = false;
			         	    	 second = true;
			         	     	 %>
						        <table id = "customers" border="1" style="width:300px">
						       		<tr id = "head">
						       			<th scope="Row">Select</th>
										<th>Pet Name</th>
										<th>Image</th>
										<th>Owner</th>
										<th>Level</th>
										
										<th>Experience</th>
										
										<th>HP</th>
										<th>Attack</th>
										<th>Defense</th>
										<th>Speed</th>
										
										<th>Battle Won</th>
										<th>Battle Lost</th>
										<th>Win Ratio</th>
									</tr>
						        <%
			         	     }
			         	     
				         	     if (second) {
					          	    second = false;
					          	 	if (cur_health>0)
					         	 	{
					          	    %><td scope="col">
					            		<input name="UserSelected" id="<%=pet_name%>" type="radio" value="<%=pet_name%>" checked='checked'>
					        		</td> <%
					        	 	}
					          	 	else
					          	 	{
						          	    %><td scope="col">
						            		<input name="UserSelected" id="<%=pet_name%>" type="radio" value="<%=pet_name%>" disabled = true>
						        		</td> <%
						        	}
					          	 }
					        	 else {
					        		 if (cur_health>0)
						         	 {
					          	    	%><td scope="col">
					            			<input name="UserSelected" id="<%=pet_name%>" type="radio" value="<%=pet_name%>">
					        			</td> <%
						         	 }
					        		 else
					        		 {
						          	   	%><td scope="col">
						            		<input name="UserSelected" id="<%=pet_name%>" type="radio" value="<%=pet_name%>" disabled = true>
						        		</td> <%
							         }
					        	 }
				         	     %>
				         			<td scope="col"><%=pet_name%></td>
				         			<td> <img alt="" src="icons/<%= avatar_url%>.gif";" /></td>
				         			<td><%=pet_owner%></td>
				         			<td><%=pet_level%></td>
				         			<td><%=cur_exp%>/<%= max_exp%> </td>
				         			<td><%=cur_health%>/<%= max_health%></td>
				         			<td><%=attack_pt%></td>
				         			<td><%=defense_pt%></td>
				         			<td><%=speed_pt%></td>
				         			<td><%=bat_won%></td>
				         			<td><%=bat_lost%> </td>
				         			<td><%=nf.format(win_rat.doubleValue())%> </td>
								</tr>
				          		<%
			         	     
			   			}
			   		} //for
		   		 	%></table><br><br>
		            <form name="input" action="" method="post">
	       				<button name="battle" type="button" onclick="myFunction('<%=user_pet_name%>')">Battle!</button>
	       			</form>
			   		<%
			    }//bigger else
			}
			 %>
			<form action="pet_stats.jsp" method="post">
				<input name="return" type="submit" value="Return" />
				<input type = "hidden" name="user" value="<%=user_name%>"/>
			</form>
	</body>
</html>