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
    	<link type="text/css" rel="stylesheet" href="/stylesheets/main.css" />
    	<script src="//code.jquery.com/jquery-1.9.1.js"></script>
    	<style>
    	h1 {color:red;}
		h2 {color:white;}
		h3 {color:Black;}
		p1 {color:black;}
		p2 {color:red;}
		p3 {color:green;}
		
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
		</style>
	</head>
  	
  	
  	<script>
  	
  	function returnFunction()
	{
		
			//alert($('input:radio[name=food_item]:checked').val());
			//alert("yo");
			document.getElementById("check2").innerHTML="Updatng database...";
			
			//document.getElementById("feed_button").disabled = true;
			//document.getElementById("return_button").disabled = true;
			
			
			var form2 = document.createElement("form");
			
			form2.name = "input";
			form2.method = "post";
			form2.action = "pet_stats.jsp";

		    //form.submit();
		    
		    var delay=3000//3 seconds
	    	setTimeout(function(){
				form2.submit();
	    	},delay)
		   	
		
	}
  	
  	
	function myFunction(my_pet_name)
	{

			document.getElementById("feed_button").disabled = true;
			
			
			var form = document.createElement("form");
			var food_value = document.createElement("input");
			var pet_name=document.createElement("input");
			
			form.name = "input";
			form.method = "post";
			form.action = "feed_pet.jsp";
			
			food_value.type="number";
			food_value.name="food_item";
			food_value.value=$('input:radio[name=food_item]:checked').val();
			
			
			pet_name.type="text";
			pet_name.name="pet_name";
			pet_name.value=my_pet_name;
			
		    form.appendChild(food_value);
		    form.appendChild(pet_name);

		    form.submit();
		    /*
		    
		    var delay=3000//3 seconds
	    	setTimeout(function(){
				form.submit();
	    	},delay)
		   	*/
		
	}
	</script>
  	

  	<body>
  	
  	
  	
  	
	<%
	//Integer food_value = 0;
	
	String pet_name = request.getParameter("pet_name");
	String food_value =request.getParameter("food_item");
	
	
	Integer cur_health = 0;
	Integer max_health = 0;
	Integer MAX_NUM_PETS=1;
	Integer DELAY=5;
	
	//database variables
	String db_pet_name = "";
	String avatar_url="";
	
	
	Integer up_health = 0;
	Integer down_food_point = 0;
	
	
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
    //List <String> pet_owners = new ArrayList<String>();
    
    
    //for checking how many food items left
    Integer cur_food_point = 0;
    
    
    DatastoreService game_statstore = DatastoreServiceFactory.getDatastoreService();
		
		
    //Filter user_only = new FilterPredicate ("user", FilterOperator.EQUAL, user_name);
	
    Query q_b_f = new Query("game_stats");
    q_b_f.setFilter(user_only);
    

    PreparedQuery pq_b_f = game_statstore.prepare(q_b_f);
    List<Entity> game_stats = pq_b_f.asList(FetchOptions.Builder.withLimit(1));
    
    if (game_stats.isEmpty()){
    	%>
        <p1><font face="verdana">No Food Points!</p1><br>
		<%

	} else {
		for (Entity game_stat: game_stats) {
	
			   //Integer cur_high_score=Integer.parseInt(game_stat.getProperty("high_score").toString());
		  	   cur_food_point=Integer.parseInt(game_stat.getProperty("food_points").toString());
		}
	}
    if (pet_stats.isEmpty()){
	    	%>
	        <p1><font face="verdana">No Pets!</p1><br>
	        
	        <form action="create.jsp" method="post">
				<h2> <input name="create" type="submit" value="Create New Monster!" /></h2>
			</form>
		
			<%
	    }
	    else {
	    
	   		for (Entity pet_stat: pet_stats) {
	   			
	   			avatar_url=pet_stat.getProperty("avatar_url").toString();
	   			//avatar_url=pet_stat.getProperty("avatar_url").toString();
		  		if (pet_stat.getProperty("pet_name").toString()==null) {
		  			db_pet_name="no name";	
		  		}
		  		else {
		  			pet_name = pet_stat.getProperty("pet_name").toString();
		  		}
		  		cur_health=Integer.parseInt(pet_stat.getProperty("cur_health").toString());
		  		max_health=Integer.parseInt(pet_stat.getProperty("max_health").toString());
		  		//FOR TESTING ONLY
		  		//max_health=10000;
		  		//cur_food_point=0;
		   		 %>
	       		<!--variable "pet_name" posted from the previous page-->
	      		<h1><font face="verdana"><%=pet_name%></h1>
				<p1> <img alt="" src="icons/<%= avatar_url%>.gif";" /></p1><br><br>
				<%
				if (food_value == null) {
				%>
				
					<p1><font face="verdana"> HP: <%=cur_health%>/<%= max_health%> </p1><br>
					<p1><font face="verdana"> Food points left: <%=cur_food_point%> </p1><br><br>
				
					<p1><font face="verdana"> Please select item to feed pet: <%=pet_name%> </p1><br>
					<form name="input" action="" method="post">
						<input type="radio" name="food_item" value="1" checked="checked">Ice Cream (+1 HP)<br>
						<input type="radio" name="food_item" value="2" >Corn (+2 HP)<br>
						<input type="radio" name="food_item" value="5">Carrots (+5 HP)<br>
						<input type="radio" name="food_item" value="7">Meatballs (+7 HP) <br> <br>
						
						<button name="feed" id="feed_button" type="button" onclick="myFunction('<%=pet_name%>')">Select and Feed Pet</button>
						
						
<%-- 						<% if ((cur_health<max_health) || (cur_food_point <1)) { %>
						
							<button name="feed" id="feed_button" type="button" onclick="myFunction('<%=pet_name%>')">Select and Feed Pet</button><br>
						<%} else { %>
							<p1> Already at max HP! </p1><br>	
							<button name="feed" type="button" disabled>Feed Pet</button><br>
						<%} %> --%>
							
					</form>
					<form action="pet_stats.jsp" method="post">
						<input name="return" type="submit" value="Return" />
						<input type = "hidden" name="user" value="<%=user_name%>"/>
					</form>

				<%
				} else {

				
				//if (food_value != null) {
					
					up_health = cur_health+Integer.valueOf(food_value);
					//down_food_point = 0;
					//hard code for testing!!!!!!!!! REMOVE LATER
					
					if (Integer.valueOf(food_value) <= cur_food_point) {
					
						if (up_health >= max_health){
						//if (up_health >= 100){
							%>
							<p1><font face="verdana"> Now at maximum HP! </p1><br>						
							<%
							up_health=max_health;
							down_food_point =  cur_food_point - Integer.valueOf(food_value);
						} else if (cur_health == max_health) {
							%>
							<p1><font face="verdana"> Already at maximum HP! </p1><br>						
							<%
							//up_health=max_health;
							up_health=cur_health;
							down_food_point=cur_food_point;
						} else {
							
							%>
							<p1><font face="verdana"> Now gaining <%=food_value%> HP. </p1><br>
							<%
							up_health = cur_health+Integer.valueOf(food_value);
							down_food_point =  cur_food_point - Integer.valueOf(food_value);
						}
					} else {
						%>
						<p1><font face="verdana"> Not enough food points! </p1><br>						
						<%
						up_health=cur_health;
						down_food_point=cur_food_point;

					}
					pet_stat.setProperty("cur_health", up_health);
					//This should always just have one value
					for (Entity game_stat: game_stats) {
						
						   //Integer cur_high_score=Integer.parseInt(game_stat.getProperty("high_score").toString());
						game_stat.setProperty("food_points", down_food_point);
					}
					petstatstore.put(pet_stats);
					game_statstore.put(game_stats);
					%>
					<p1><font face="verdana"> Please return. </p1><br>
					<form name="input" action="" method="post">
						<button name="initiate" id="init" type="button" onclick="returnFunction()">Return</button><br>
						<p id="check2"></p>
					</form>
					
					
					<%
				}
	   		}
	    }
	%>
	</body>

</html>
