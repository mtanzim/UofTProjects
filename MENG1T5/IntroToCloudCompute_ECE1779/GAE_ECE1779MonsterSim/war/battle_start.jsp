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

<%@ page import="java.util.ArrayList" %>
<%@ page import="java.awt.event.ActionListener" %>
<%@ page import="java.awt.event.ActionEvent" %>


<html>

	<head>
    	<link type="text/css" rel="stylesheet" href="/stylesheets/main.css" />
    	<style type="text/css">
    	
    	.CSSTableGenerator {
		margin: 0 auto;padding:0px;
		box-shadow: 10px 10px 5px #888888;
		border:1px solid #bf5f00;
		
		border-radius:13px;
		}
		.CSSTableGenerator table{
		    border-collapse: collapse;
		        border-spacing: 0;
			width:100%;
			height:100%;
			margin:0px;padding:0px;
		}
		.CSSTableGenerator tr:last-child td:last-child {
			-moz-border-radius-bottomright:13px;
			-webkit-border-bottom-right-radius:13px;
			border-bottom-right-radius:13px;
		}
		.CSSTableGenerator table tr:first-child td:first-child {
			-moz-border-radius-topleft:13px;
			-webkit-border-top-left-radius:13px;
			border-top-left-radius:13px;
		}
		.CSSTableGenerator table tr:first-child td:last-child {
			-moz-border-radius-topright:13px;
			-webkit-border-top-right-radius:13px;
			border-top-right-radius:13px;
		}
		.CSSTableGenerator tr:last-child td:first-child{
			-moz-border-radius-bottomleft:13px;
			-webkit-border-bottom-left-radius:13px;
			border-bottom-left-radius:13px;
		}
		.CSSTableGenerator tr:nth-child(odd){ background-color:#ffd4aa; }
		.CSSTableGenerator tr:nth-child(even)    { background-color:#ffd4aa; }
		.CSSTableGenerator td{
			vertical-align:middle;
			border:1px solid #bf5f00;
			border-width:0px 1px 1px 0px;
			text-align:center;
			padding:7px;
			font-size:15px;
			font-family:Helvetica;
			font-weight:bold;
			color:#000000;
		}
		.CSSTableGenerator tr:last-child td{
			border-width:0px 1px 0px 0px;
		}
		.CSSTableGenerator tr td:last-child{
			border-width:0px 0px 1px 0px;
		}
		.CSSTableGenerator tr:last-child td:last-child{
			border-width:0px 0px 0px 0px;
		}
		.CSSTableGenerator tr:first-child td{
			
		    border-top-right-radius:13px;
		    border-top-left-radius:13px;

			border:0px solid #bf5f00;
			text-align:center;
			border-width:0px 0px 1px 1px;
			font-size:14px;
			font-family:Arial;
			font-weight:bold;
			color:#000000;
		}
		.CSSTableGenerator tr:first-child td:first-child{
			border-width:0px 0px 1px 0px;
		}
		.CSSTableGenerator tr:first-child td:last-child{
			border-width:0px 0px 1px 1px;
		}
    	
    	
    	.flip-horizontal {
		    -moz-transform: scaleX(-1);
		    -webkit-transform: scaleX(-1);
		    -o-transform: scaleX(-1);
		    transform: scaleX(-1);
		    -ms-filter: fliph; /*IE*/
		    filter: fliph; /*IE*/
		}
		
		html { 
			background: url(battle_background.png) no-repeat center center fixed; 
			
			-webkit-background-size: cover; 
			-moz-background-size: cover; 
			-o-background-size: cover; 
			background-size: cover; 
		} 
		
		h1 {color:red; text-align:center;}
		h2 {color:blue; text-align:center;}
		p1 {color:black; text-align:center;}
		p2 {color:red; text-align:center;}
		p3 {color:green; text-align:center;}
		</style>
  	</head>

  	<body>
  	<div style="text-align: center">
  	
  			<%!
				//INITIALIZE PET VARIABLES
		  		public  Integer MAX_NUM_PETS=3;
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
				public  Integer user_speed = 0;//storing for battle user stats
				public	Integer user_atk = 0;
				public 	Integer user_def = 0;
				public	String user_type = "";
				public	Integer user_lvl = 0;
				public	Integer user_hp = 0;
				public 	Integer opp_lvl = 0;//storing for battle opp stats
				public 	String opp_type = "";
				public	Integer opp_hp = 0;
				public  Integer opp_speed = 0;
				public	Integer opp_atk = 0;
				public 	Integer opp_def = 0;
				public	String opp_pet_name = "";
				public  Integer bat_won=0;
				public  Integer bat_lost=0;
		  		public  Double win_rat=0.0;	

  			%>
  			
  			
  			<%!//function that sets variables after getting them from DB, per monster
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
			<%!//whos attacking, from db
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
			<%!//Function to set battle to start or not
			public void requestResponse (String user_pet_name, String opp_pet_name, String nextattacker)
			{
				int result = 0;
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
					result = 1;
			   	}
			   	else 
			   	{
		  			for (Entity pet_stat: pet_stats) 
		  			{
		  					pet_stat.setProperty("nextattacker", nextattacker);
		  					if (nextattacker.equals("done"))//if rejected
		  					{
		  						if (pet_stat.getProperty("pet_1").toString().equals(user_pet_name))
		  	   					{//set the pet's name to empty so cleanup doesnt remove it
		  							//this is so that if player A waiting for attack and player B finishes game and
		  							//goes to battle_search, player B doesnt remove the battle so taht player A can remove
		  							//the battle when it goes to battle search
		  	   						pet_stat.setProperty("pet_1", "");
		  	   					}
		  	   					else
		  	   					{
		  	   						pet_stat.setProperty("pet_2", "");
		  	   					}
		  					}
		  					petstatstore.put(pet_stats);
							result = 3;
					}
		  			if (nextattacker.equals("done"))
		  			{//battle over, remove opp_name things from pet_stats
		  				pet_1_filter = new FilterPredicate ("pet_name", FilterOperator.EQUAL, user_pet_name);
		  			    pet_2_filter = new FilterPredicate ("pet_name", FilterOperator.EQUAL, opp_pet_name);
		  			    combined_filter = CompositeFilterOperator.or(pet_1_filter, pet_2_filter);
		  			    q_b = new Query("pet_stats");
		  			    q_b.setFilter(combined_filter);
		  				pq_b = petstatstore.prepare(q_b);
		  			    pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(5));
		  			    
			  			if (pet_stats.isEmpty())
			  			{
			  	    	}
			  	    	else 
			  	    	{
			  	   			for (Entity pet_stat: pet_stats) 
			  	   			{
		  	   					pet_stat.setProperty("opp_name", "");
		  	   					petstatstore.put(pet_stat);
			  	   			}
			  			}
		  			}
			   	}
			}
			%>
  			<%
  			UserService userService = UserServiceFactory.getUserService();
  	 		User user = userService.getCurrentUser();
			NumberFormat nf = NumberFormat.getInstance();
			nf.setMaximumFractionDigits(2);
			nf.setMinimumFractionDigits(2);
			
			//get user pet from prev page
			String user_pet_name = request.getParameter("user");

		    DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();

		    Filter pet_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, user_pet_name);
		    Query q = new Query("pet_stats");
		    q.setFilter(pet_only);
		    PreparedQuery pq = petstatstore.prepare(q);
		    List<Entity> pet_stats = pq.asList(FetchOptions.Builder.withLimit(MAX_NUM_PETS)); 
       	 	String op_name=request.getParameter("opponent");
		%>
		
  			<h1><font face="verdana"><%=user_pet_name %> vs <%=op_name%>!</h1>
		  	<div  style="float: left;text-align: center; width: 50%">
		  	<h2><font face="verdana">Selected Pet</h2>
		 <% 	
			if (pet_stats.isEmpty()){
		    	%>
		        <p1><font face="verdana">No Pets!</p1>
		        <form action="create.jsp" method="post">
					<h2><font face="verdana"> <input name="create" type="submit" value="Create New Monster!" /></h2>
				</form>
				<%
		    }
		    else {
		    
		   		for (Entity pet_stat_root : pet_stats) {
					set_pet_variables (pet_stat_root);
					user_speed = speed_pt;
					user_atk = attack_pt;
					user_def = defense_pt;
					user_type = avatar_url;
					user_lvl = pet_level;
					user_hp = cur_health;
          		 	if (avatar_url.contains("fire")) {
          		    %>
          		   	<table class="CSSTableGenerator">
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana"><%=user_pet_name %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                               <img  src="icons/<%= avatar_url%>breath.gif">
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana">Level: <%=pet_level %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                               <font face="verdana">Health: <%=cur_health%>/<%= max_health%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                               <font face="verdana">Attack: <%=attack_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana">Defence: <%=defense_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana">Speed: <%=speed_pt %>
                            </td>
                        </tr>
                    </table>
                    <%
          	        }
          	        if (avatar_url.contains("water")) {
          		   	%>
          		   	<table class="CSSTableGenerator">
                        <tr >
                            <td style="background-color:#9db7f4;">
                                <font face="verdana"><%=user_pet_name %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                               <img  src="icons/<%= avatar_url%>breath.gif">
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                                <font face="verdana">Level: <%=pet_level %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                               <font face="verdana">Health:<%=cur_health%>/<%= max_health%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                               <font face="verdana">Attack:  <%=attack_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                                <font face="verdana">Defence: <%=defense_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                                <font face="verdana">Speed: <%=speed_pt %>
                            </td>
                        </tr>
                    </table>
                    <%
          	        }
          	        if (avatar_url.contains("earth")) {
          		   	%>
          		   	<table class="CSSTableGenerator">
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana"><%=user_pet_name %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                               <img  src="icons/<%= avatar_url%>breath.gif">
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana">Level: <%=pet_level %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                               <font face="verdana">Health:<%=cur_health%>/<%= max_health%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                               <font face="verdana">Attack:  <%=attack_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana">Defence: <%=defense_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana">Speed: <%=speed_pt %>
                            </td>
                        </tr>
                    </table>
                    <%
          	        }
		   		}
		    }			
     %>
     
     		</div>
     		<div  style="float: right;text-align: center; width: 50%">
       		<h2><font face="verdana">Opponent</h2>
       		<% 
		    Filter op_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, op_name);
		    Filter op_not_user = op_only;//CompositeFilterOperator.and(op_only, not_user_only);
		    
		    Query q_b = new Query("pet_stats");
		    q_b.setFilter(op_not_user);
		    PreparedQuery pq_b = petstatstore.prepare(q_b);
		    List<Entity> op_stats = pq_b.asList(FetchOptions.Builder.withLimit(20)); 
			if (op_stats.isEmpty()){
		    	%>
		        <p1><font face="verdana">No opponents found!</p1><br>
				<%
		    }
		    else {
		    
		   		for (Entity op_stat_root : op_stats) {
					set_pet_variables (op_stat_root);
					opp_pet_name = pet_name;
					opp_speed = speed_pt;
					opp_atk = attack_pt;
					opp_def = defense_pt;
					opp_type = avatar_url;
					opp_lvl = pet_level;
					opp_hp = cur_health;
	          		if (avatar_url.contains("fire")) {
          		   	%>
          		   	<table class="CSSTableGenerator">
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana"><%=pet_name %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                               <img  src="icons/<%= avatar_url%>breath.gif">
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana">Level: <%=pet_level %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                               <font face="verdana">Health: <%=cur_health%>/<%= max_health%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                               <font face="verdana">Attack: <%=attack_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana">Defence: <%=defense_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#f5ac79;">
                                <font face="verdana">Speed: <%=speed_pt %>
                            </td>
                        </tr>
                    </table>
                    <%
          	        }
          	        if (avatar_url.contains("water")) {
          		   	%>
          		   	<table class="CSSTableGenerator">
                        <tr >
                            <td style="background-color:#9db7f4;">
                                <font face="verdana"><%=pet_name %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                               <img  src="icons/<%= avatar_url%>breath.gif">
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                                <font face="verdana">Level: <%=pet_level %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                               <font face="verdana">Health:<%=cur_health%>/<%= max_health%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                               <font face="verdana">Attack:  <%=attack_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                                <font face="verdana">Defence: <%=defense_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#9db7f4;">
                                <font face="verdana">Speed: <%=speed_pt %>
                            </td>
                        </tr>
                    </table>
                    <%
          	        }
          	        if (avatar_url.contains("earth")) {
          		   	%>
          		   	<table class="CSSTableGenerator">
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana"><%=pet_name %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                               <img  src="icons/<%= avatar_url%>breath.gif">
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana">Level: <%=pet_level %>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                               <font face="verdana">Health:<%=cur_health%>/<%= max_health%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                               <font face="verdana">Attack:  <%=attack_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana">Defence: <%=defense_pt%>
                            </td>
                        </tr>
                        <tr>
                            <td style="background-color:#d2c17c;">
                                <font face="verdana">Speed: <%=speed_pt %>
                            </td>
                        </tr>
                    </table>
                    <%
          	        }
		   		}
		    }
			%>
			</div>
<%-- 			<p1>their speed is <%=opp_speed %> your speed is <%=user_speed %></p1><br> --%>
			<%
			String whoattacked = whoAttacking(user_pet_name);//nextattacker from db
			String responseToBattle = request.getParameter("whoattack");//only useful if is request state
			String nextattacker = request.getParameter("nextattacker");
			String doyouattack = whoAreYou(user_pet_name);//who are you
			if (whoattacked.equals("done"))
			{
			%>
<!-- 			<h2>battle over! disregard info below and return to main page</h2>  -->
			<% 
			}
			else if (whoattacked.isEmpty())//first turn, right from search_battle
			{
				%>
				Requesting Battle with <%=op_name %> <br>
				<%
				//update db to show battling monsters (REMOVE THEM FROM BEING SEARCHABLE IN search_battle)
									//save to db 
							        Entity pet_stats1 = new Entity("pet_battle", user_pet_name);
							        
							        pet_stats1.setProperty("pet_1", user_pet_name);
							        pet_stats1.setProperty("pet_2", op_name);
							        pet_stats1.setProperty("nextattacker","request");

							        //DatastoreService 
							        petstatstore = DatastoreServiceFactory.getDatastoreService();
							        petstatstore.put(pet_stats1);
							        
							        //update name of opponent monster in each monsters' pet_stats
							        for (Entity op_stat_root : op_stats) {
							        	op_stat_root.setProperty("opp_name", user_pet_name);
							        	petstatstore = DatastoreServiceFactory.getDatastoreService();
								        petstatstore.put(op_stat_root);
							        }
							        
							        for (Entity pet_stat_root : pet_stats) {
							        	pet_stat_root.setProperty("opp_name", op_name);
							        	petstatstore = DatastoreServiceFactory.getDatastoreService();
								        petstatstore.put(pet_stat_root);
							        }
							        
							        ////////////done saving to db
							        
			}
			
			nextattacker = whoAttacking(user_pet_name);
			if (nextattacker.equals("done"))
			{
				%><h2>You Lost! Return to Main Page</h2> <%
			}
			else if (nextattacker.equals(doyouattack))
			{
				%><h2>You Are Attacking!</h2> <%
			}
			else if (nextattacker.equals("request"))
			{
				%><h2>Wait for Battle Response</h2> <%
			}
			else
			{
				%><h2>Wait for Opponent to Attack</h2> <%
			}
			
			%>
<%-- 			nextattacker = <%=nextattacker%><br> --%>
<%-- 			requestToBattle = <%=responseToBattle %><br> --%>
			<% 
			//String doyouattack = whoAreYou(user_pet_name);
			%>
<%-- 			you are <%=doyouattack%><br> --%>
			<% 
			if ( !whoattacked.equals("request") & !whoattacked.equals("done") & doyouattack.equals(nextattacker)) 
				//not in done or request state and user is atacking next
			{
			%>
			
			<form action="battle_attack.jsp" method="post">
				<h2> 
				<input name="whoattack" type="hidden" value="<%=nextattacker%>" />
				<input name="whoareyou" type="hidden" value="<%=doyouattack%>" />
				<input name="opponent" type="hidden" value=<%=op_name %> />
				<input name="user" type="hidden" value=<%=user_pet_name %> />
				
				<input name="user_speed" type="hidden" value=<%=user_speed %> />
				<input name="user_atk" type="hidden" value=<%=user_atk %> />
				<input name="user_def" type="hidden" value=<%=user_def %> />
				<input name="user_type" type="hidden" value=<%=user_type %> />
				<input name="user_lvl" type="hidden" value=<%=user_lvl %> />
				<input name="user_hp" type="hidden" value=<%=user_hp %> />
				
				<input name="opp_lvl" type="hidden" value=<%=opp_lvl %> />
				<input name="opp_type" type="hidden" value=<%=opp_type %> />
				<input name="opp_hp" type="hidden" value=<%=opp_hp %> />
				<input name="opp_speed" type="hidden" value=<%=opp_speed %> />
				<input name="opp_atk" type="hidden" value=<%=opp_atk %> />
				<input name="opp_def" type="hidden" value=<%=opp_def %> />
				
				<input name="attacktype" type="hidden" value="weak" />
				<input name="create" type="submit" value="WEAK ATTACK! " /> 100% Hit Rate</h2>
			</form>
			
			<form action="battle_attack.jsp" method="post">
				<h2> 
				<input name="whoattack" type="hidden" value="<%=nextattacker%>" />
				<input name="whoareyou" type="hidden" value="<%=doyouattack%>" />
				<input name="opponent" type="hidden" value=<%=op_name %> />
				<input name="user" type="hidden" value=<%=user_pet_name %> />
				
				<input name="user_speed" type="hidden" value=<%=user_speed %> />
				<input name="user_atk" type="hidden" value=<%=user_atk %> />
				<input name="user_def" type="hidden" value=<%=user_def %> />
				<input name="user_type" type="hidden" value=<%=user_type %> />
				<input name="user_lvl" type="hidden" value=<%=user_lvl %> />
				<input name="user_hp" type="hidden" value=<%=user_hp %> />
				
				<input name="opp_lvl" type="hidden" value=<%=opp_lvl %> />
				<input name="opp_type" type="hidden" value=<%=opp_type %> />
				<input name="opp_hp" type="hidden" value=<%=opp_hp %> />
				<input name="opp_speed" type="hidden" value=<%=opp_speed %> />
				<input name="opp_atk" type="hidden" value=<%=opp_atk %> />
				<input name="opp_def" type="hidden" value=<%=opp_def %> />
				
				<input name="attacktype" type="hidden" value="normal" />
				<input name="create" type="submit" value="ATTACK!" /> 70% Hit Rate</h2>
			</form>
			
			<form action="battle_attack.jsp" method="post">
				<h2> 
				<input name="whoattack" type="hidden" value="<%=nextattacker%>" />
				<input name="whoareyou" type="hidden" value="<%=doyouattack%>" />
				<input name="opponent" type="hidden" value=<%=op_name %> />
				<input name="user" type="hidden" value=<%=user_pet_name %> />
				
				<input name="user_speed" type="hidden" value=<%=user_speed %> />
				<input name="user_atk" type="hidden" value=<%=user_atk %> />
				<input name="user_def" type="hidden" value=<%=user_def %> />
				<input name="user_type" type="hidden" value=<%=user_type %> />
				<input name="user_lvl" type="hidden" value=<%=user_lvl %> />
				<input name="user_hp" type="hidden" value=<%=user_hp %> />
				
				<input name="opp_lvl" type="hidden" value=<%=opp_lvl %> />
				<input name="opp_type" type="hidden" value=<%=opp_type %> />
				<input name="opp_hp" type="hidden" value=<%=opp_hp %> />
				<input name="opp_speed" type="hidden" value=<%=opp_speed %> />
				<input name="opp_atk" type="hidden" value=<%=opp_atk %> />
				<input name="opp_def" type="hidden" value=<%=opp_def %> />
				
				<input name="attacktype" type="hidden" value="strong" />
				<input name="create" type="submit" value="STRONG ATTACK!" /> 40% Hit Rate</h2>
			</form>
			<%
			}
			else if (whoattacked.equals("request"))//someone is waiting for a response to battle
			{
				
				if (responseToBattle.equals("accept"))//battle begun!
				{
					%>
					BATTLE ACCEPTED! <br>
					<%
					if (user_speed>=opp_speed)
					{
						if (doyouattack.equals("you"))
							nextattacker="you";
						else if (doyouattack.equals("them"))
							nextattacker="them";
						
					}
					else
					{
						if (doyouattack.equals("you"))
							nextattacker="them";
						else if (doyouattack.equals("them"))
							nextattacker="you";
					}
					requestResponse(user_pet_name,op_name,nextattacker);
				}
				else if (responseToBattle.equals("reject"))//battle canceled/rejected
				{
					%>
					BATTLE CANCELED! <br>
					<%
					nextattacker="done";
					requestResponse(user_pet_name,op_name,nextattacker);
				}
				else
				{
					%>
					Please wait for a response from <%=op_name %> <br>
					<%
				}
				
			}
			else if ( !whoattacked.equals("request") & !whoattacked.equals("done") & !doyouattack.equals(nextattacker))
				//in battle, not your turn
			{
			%>
<%-- 				please wait for <%=nextattacker %> to attack!<br> --%>
			<%
			}
			%>
		<form action="pet_stats.jsp" method="post">
			<h2> <input name="return" type="submit" value="Return to Main Page" /></h2>
		</form>
	</div>
	</body>
</html>
