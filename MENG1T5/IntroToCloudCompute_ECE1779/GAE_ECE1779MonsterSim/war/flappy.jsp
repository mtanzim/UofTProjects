<%@ page import="ece1779.monstersim.monster" %>
<%@ page import="java.util.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.jsp.*"%>
<%@ page import="javax.servlet.http.*" %>


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

<%@ page import="java.util.ArrayList" %>



<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<html>
	<head>
	<style>
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
	
	<%
	
	String pet_name = request.getParameter("pet_name");
	//String food_value =request.getParameter("food_item");
	String avatar_url="";
	
	Integer MAX_NUM_PETS=1;
	
	UserService userService = UserServiceFactory.getUserService();
	User user = userService.getCurrentUser();
	
	DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();

	String user_name=user.getNickname();
    Filter pet_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, pet_name);
    Filter user_only = new FilterPredicate ("user", FilterOperator.EQUAL, user_name);
    
 	Filter op_not_user = CompositeFilterOperator.and(pet_only, user_only);
    
    Query q_b = new Query("pet_stats");
    q_b.setFilter(op_not_user);
    

    PreparedQuery pq_b = petstatstore.prepare(q_b);
    List<Entity> pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(MAX_NUM_PETS)); 
    
    if (pet_stats.isEmpty()){
    	response.sendRedirect("create.jsp");
    }
    else {
    	for (Entity pet_stat : pet_stats) {
   			//pet_owners.add(pet_stat.getKey().toString());
   			
   			avatar_url=pet_stat.getProperty("avatar_url").toString();
    	}
    	
    }
	
	%>
	
	
		
		<h1><font face="verdana">FlappyMon's Moonwalker</h1>
		<p1><font face="verdana"> Play this game to collect food points! </p1><br><br>
		<canvas id="gameCanvas" width="650" height="600" style="border:2px solid #000000;">
			Your browser does not support the HTML5 canvas tag.
		</canvas>
		
		<script id="helper" data-name="<%= avatar_url%>" src="game2/flappy.js"></script>
		<script src="game2/flappy.js"></script>
		
		<form name="input" action="" method="post">
       			<button name="reg_score" type="button" onclick="myFunction()">Submit!</button><br>
       			<p1 id="comment"><font face="verdana"></p1><br>
       			<p1 id="high_score"><font face="verdana"></p1><br>
       			<p1 id="tot_score"><font face="verdana"></p1><br>
     	</form>
		

		
		
		
	
	
	</body>
	
</html>