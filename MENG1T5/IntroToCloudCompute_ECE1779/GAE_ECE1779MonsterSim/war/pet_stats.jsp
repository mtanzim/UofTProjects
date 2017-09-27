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

<html>

	<head>
    	<link type="text/css" rel="stylesheet"/>
    	<script src="//code.jquery.com/jquery-1.9.1.js"></script>
    	<script type="text/javascript">
    	function myFunction(){
    	    //alert($('input:radio[name=UserSelected]:checked').val());
			var form = document.createElement("form");
			form.name = "input";
			form.method = "post";
			form.action = "battle_search.jsp";
			var inputname = document.createElement("input");
			var oppname = document.createElement("input");
			oppname.type="text";
			oppname.name="opp_pet_name";
			oppname.value="";
			inputname.type = "text";
			inputname.name = "pet_name";
			inputname.value = $('input:radio[name=UserSelected]:checked').val();
		    form.appendChild(inputname);
		    form.appendChild(oppname);
		    form.submit(); 
    	}
    	</script>
    	
    	<script type="text/javascript">
    	function myFunction_play(){
    	    //alert($('input:radio[name=UserSelected]:checked').val());
			var form = document.createElement("form");
			form.name = "input";
			form.method = "post";
			form.action = "flappy.jsp";
			var inputname = document.createElement("input");
			var oppname = document.createElement("input");
			
			inputname.type = "text";
			inputname.name = "pet_name";
			inputname.value = $('input:radio[name=UserSelected]:checked').val();
		    form.appendChild(inputname);

		    form.submit(); 
    	}
    	</script>
    	
    	<script type="text/javascript">
    	function myFunction_feed(){
    	    //alert($('input:radio[name=UserSelected]:checked').val());
			var form = document.createElement("form");
			form.name = "feed";
			form.method = "post";
			form.action = "feed_pet.jsp";
			var inputname = document.createElement("input");
			//var oppname = document.createElement("input");
			//oppname.type="text";
			//oppname.name="opp_pet_name";
			//oppname.value="";
			inputname.type = "text";
			inputname.name = "pet_name";
			inputname.value = $('input:radio[name=UserSelected]:checked').val();
		    form.appendChild(inputname);
		    //form.appendChild(oppname);
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
  
 <%
 		//INITIALIZE PET VARIABLES
		Integer MAX_NUM_PETS=3;
		boolean alternate = false;
		//all of the below variables will come from the database
		String avatar_url="";
		String pet_name = "";
		String pet_owner ="";
		String row_name = "";
		String opp_name = "";
		Integer pet_level = 0;
		Integer max_exp=0;
		Integer cur_exp=0;
		Integer cur_health=0;
		Integer max_health=0;
		Integer defense_pt = 0;
		Integer attack_pt = 0;
		Integer speed_pt = 0;
		Integer bat_won=0;
		Integer bat_lost=0;
		//Double win_rat = (double)bat_won/(double)(bat_lost+bat_won);
		Double win_rat=0.0;

 		UserService userService = UserServiceFactory.getUserService();
 		User user = userService.getCurrentUser();
 		pageContext.setAttribute("user", user);
 	 	if (user == null) {
 	     	pageContext.setAttribute("user", user);
 	     	response.sendRedirect("login.jsp");
 	   	}
 	 	
 		%>
 		<p>Hello, ${user.nickname} (You can
 		<a href="<%= userService.createLogoutURL(request.getRequestURI()) %>">sign out</a>.)</p>
 		<%
 		
		 NumberFormat nf = NumberFormat.getInstance();
		 nf.setMaximumFractionDigits(2);
		 nf.setMinimumFractionDigits(2);

		 String user_name=user.getNickname();

	    DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
	    Filter user_only = new FilterPredicate ("user", FilterOperator.EQUAL, user_name);
	    Query q = new Query("pet_stats");
	    q.setFilter(user_only);
	    PreparedQuery pq = petstatstore.prepare(q);
	    List<Entity> pet_stats = pq.asList(FetchOptions.Builder.withLimit(5)); 
	    List <String> pet_owners = new ArrayList<String>();
	    
	    if (pet_stats.isEmpty()){
	    	response.sendRedirect("create.jsp");
	    }
	    else {
	        %>
	        <table id = "customers" border="1" style="width:300px">
	       		<tr id = "head">
	       			<th scope="Row">Select</th>
					<th>Pet Name</th>
					<th>Image</th>
					<th>Level</th>
					
					<th>Experience</th>
					
					<th>HP</th>
					<th>Attack</th>
					<th>Defense</th>
					<th>Speed</th>
					
					<th>Battle Won</th>
					<th>Battle Lost</th>
					<th>Win Ratio</th>
					<th>Status</th>
				</tr>
	        <%
	        boolean first = true;
	   		for (Entity pet_stat : pet_stats) {
	   			//pet_owners.add(pet_stat.getKey().toString());
	   			
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
          		row_name = pet_owner+pet_name;
          		
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
          		opp_name=pet_stat.getProperty("opp_name").toString();
          		
          		if ((bat_won+bat_lost)==0){
          			win_rat=0.0;
          		}
          		else {
          			win_rat = (double)bat_won/(double)(bat_lost+bat_won);
          		}

          		
          	   	%><tr id="<%=row_name%>"><%
         
          	    if (first) {
          	    	first = false;
          	    	%><td scope="col">
            			<input name="UserSelected" id="<%=pet_name%>" type="radio" value="<%=pet_name%>" checked='checked'>
        			</td> <%
        	    }
        	    else {
          	    	%><td scope="col">
            			<input name="UserSelected" id="<%=pet_name%>" type="radio" value="<%=pet_name%>">
        			</td> <%
        	    }
          	       %>
         			<td scope="col"><%=pet_name%></td>
         			<td> <img alt="" src="icons/<%= avatar_url%>.gif";" /></td>
         			<td><%=pet_level%></td>
         		
         			<td><%=cur_exp%>/<%= max_exp%> </td>

         			<td><%=cur_health%>/<%= max_health%></td>
         			<td><%=attack_pt%></td>
         			<td><%=defense_pt%></td>
         			<td><%=speed_pt%></td>

         			<td><%=bat_won%></td>
         			<td><%=bat_lost%> </td>
         			<td><%=nf.format(win_rat.doubleValue())%> </td>
         			
         			<% 
         			if(opp_name.equals("")) {
         				%><td>Free</td><%
         			}
         			else {
         				%><td>In Battle!</td><%
         			}
 
         			%>

				</tr>
				<% 
			}	
            %>
            </table>
       		<h2><font face="verdana">Actions</h2>
       		
       		<form action="feed" action="" method="post">
        			<button name="feed_pet" type="button" onclick="myFunction_feed()">Feed Pet</button>
       		</form>
       		
       		<form name="input" action="" method="post">
       			<button name="battle" type="button" onclick="myFunction()">Battle!</button>
       		</form>
       		
       		<form action="flappy.jsp" method="post">
       			<button name="flappy" type="button" onclick="myFunction_play()">FlappyMon's Moonwalker</button>
       		</form>
       		
       		<form action="create.jsp" method="post">
       			<input name="create" type="submit" value="Create New Monster!" />
       		</form>
         		          		
         		<% 		
	    }    	
     %>
 	 
	</body>

</html>