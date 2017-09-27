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



<%@ page import="com.google.appengine.api.datastore.PreparedQuery" %>
<%@ page import="com.google.appengine.api.datastore.Query.Filter" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterPredicate" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterOperator" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilter" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilterOperator" %>
<%@ page import="com.google.appengine.api.datastore.Query.SortDirection"%>


<%@ page import="java.util.ArrayList" %>



<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>




<html>

	<head>
    	<script src="//code.jquery.com/jquery-1.9.1.js"></script>
    	<style>
    	    	
    	h1 {color:red;}
		h2 {color:blue;}
		p1 {color:black;}
		p2 {color:red;}
		p3 {color:green;}
    	
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
  	<script>
  	
  	function checkFirstVisit() {
  		
  		if (sessionStorage.getItem("is_reloaded")) {
  			alert('Reloaded!');
  			document.getElementById("is_refresh").innerHTML=1;
  			
  		}
  		else {
 		    // not first visit, so alert
 		    //alert('You refreshed!');
 		    document.getElementById("is_refresh").innerHTML=0;
 		  }
  			
  		sessionStorage.setItem("is_reloaded", true);
  		
  		/*
		  if(document.cookie.indexOf('mycookie')==-1) {
		    // cookie doesn't exist, create it now
		    document.cookie = 'mycookie=1';
		    document.getElementById("is_refresh").innerHTML=0;
		  }
		  else {
		    // not first visit, so alert
		    alert('You refreshed!');
		    document.getElementById("is_refresh").innerHTML=1;
		  }
  		
  		*/
	}
  	</script>
  	
  	
 	<body>

 		<%
 		
 		
 		String high_score = request.getParameter("high_score");
 		String tot_score = request.getParameter("tot_score");
 		//String prevent_refresh = request.getParameter("refresh");
 		
 		Integer food_points=Integer.valueOf(tot_score);
 		Integer high_score_bonus = Integer.valueOf(high_score)*3;
 		
 		Integer total_food_points = food_points + high_score_bonus;
 		
		UserService userService = UserServiceFactory.getUserService();
		User user = userService.getCurrentUser();
		String user_name=user.getNickname();
 		
 		%>
<%-- 		<h1>Refresh: <%=prevent_refresh%></h1> --%>
 		<h1><font face="verdana">User: <%=user_name%></h1>
 		
 		<h2><font face="verdana">Game Achievements</h2>
 		<p1><font face="verdana">Total Score: <%=tot_score%></p1><br>
 		<p1><font face="verdana">High Score: <%=high_score%><br><br></p1>
 		
 		
 		<p1><font face="verdana">Food Collected: <%=food_points%></p1><br>
 		<p1><font face="verdana">High Score Bonus: <%=high_score_bonus%></p1><br>
 		<p1><font face="verdana">Total Food Earned: <%=total_food_points%></p1><br>
 		
		<%

		
		DatastoreService game_statstore = DatastoreServiceFactory.getDatastoreService();
		
		
	    Filter user_only = new FilterPredicate ("user", FilterOperator.EQUAL, user_name);
		
	    Query q_b = new Query("game_stats");
	    q_b.setFilter(user_only);
	    

	    PreparedQuery pq_b = game_statstore.prepare(q_b);
	    List<Entity> game_stats = pq_b.asList(FetchOptions.Builder.withLimit(1));
	    
	    if (game_stats.isEmpty()){
	    	%>
	        <p1><font face="verdana">No Scores for user. Creating.</p1>
		
			<%
			Entity new_game_stats = new Entity("game_stats", user_name);
       		new_game_stats.setProperty("user",user_name);
       		new_game_stats.setProperty("high_score",high_score);
       		new_game_stats.setProperty("food_points",total_food_points);
       		//Date date = new Date();
       		//new_game_stats.setProperty("date", date);
	      	game_statstore.put(new_game_stats);
	      	
	      	%>
	        <p1><font face="verdana">Completed.</p1>
		
			<%
	    } else {
	    	for (Entity game_stat: game_stats) {
	    		
	    		//if (session.getAttribute("recordInsertedSuccessfully") == null )
	    		//{
	    		   Integer cur_high_score=Integer.parseInt(game_stat.getProperty("high_score").toString());
			  	   Integer cur_food_point=Integer.parseInt(game_stat.getProperty("food_points").toString());
	    		   //session.putAttribute("recordInsertedSuccessfully","true");
	    		//} else {
	    		   //case of form re-submission
	    		//}
	    		
		    	//Integer cur_high_score=Integer.parseInt(game_stat.getProperty("high_score").toString());
		  		//Integer cur_food_point=Integer.parseInt(game_stat.getProperty("food_points").toString());
		  		
		  		
		  		
		  		Integer new_food_points = cur_food_point + total_food_points;
		  		Integer new_high_score= cur_high_score;
		  		
		  		if (Integer.valueOf(high_score) > cur_high_score) {
		  			new_high_score = Integer.valueOf(high_score);
		  		}
		  		
		  		game_stat.setProperty("high_score",new_high_score);
	       		game_stat.setProperty("food_points",new_food_points);
	       		
	       		game_statstore.put(game_stat);
		  		
		  		%>
				
		 		<h2><font face="verdana">Current Stats</h2>
		 		<p1><font face="verdana">Available Food Points: <%=cur_food_point%></p1><br>
		 		<p1><font face="verdana">Record High Score: <%=cur_high_score%><br></p1><br>
		 		
		 		<h2><font face="verdana">Updating Stats To</h2>
		 		<p1><font face="verdana">Available Food Points: <%=new_food_points%></p1><br>
		 		<p1><font face="verdana">Record High Score: <%=new_high_score%><br></p1>
		 		
		 		<h1><font face="verdana">Leaderboards</h1>
		 		
				<%
	    	}
	    	
	    }
	    
	    
	    Integer MAX_NUM_SCORES=5;
	    
	    Query q_hs = new Query("game_stats").addSort("high_score",SortDirection.DESCENDING);
	    //q_hs.setFilter(user_only);
	    

	    PreparedQuery pq_hs = game_statstore.prepare(q_hs);
	    List<Entity> hs_stats = pq_hs.asList(FetchOptions.Builder.withLimit(MAX_NUM_SCORES));
	    
	    if (hs_stats.isEmpty()){
	    	%>
	        <p>No high scores recorded yet!</p>
		
			<%
	    } else {
	    	%>
	    	 <table id = "customers" border="1" style="width:300px">
	       		<tr id = "head">
					<th>User Name</th>
					<th>Score</th>
				</tr>
	    	<%
	    	for (Entity hs_stat: hs_stats) {
	    		
	    		String hs_user=hs_stat.getProperty("user").toString();
		  		Integer record_score=Integer.valueOf(hs_stat.getProperty("high_score").toString());
		  		
		  		%><tr id="vals">
		  		<td><%=hs_user%></td>
         		<td><%=record_score%></td>
         		</tr>
				<%
	    	}
	    	%> </table><br><%
	    }
 		%>

	<form name="input" action="pet_stats.jsp" method="post">
		<input name="return" id="init" value="Return" type="submit"></button><br>
	</form>	
	</body>

</html>
 		
  	  	