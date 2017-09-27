<%@ page import="ece1779.monstersim.monster" %>
<%@ page import="java.util.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.jsp.*"%>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="com.google.appengine.api.users.User" %>
<%@ page import="com.google.appengine.api.users.UserService" %>
<%@ page import="com.google.appengine.api.users.UserServiceFactory" %>
<%@ page import="com.google.appengine.api.datastore.DatastoreServiceFactory" %>
<%@ page import="com.google.appengine.api.datastore.DatastoreService" %>
<%@ page import="com.google.appengine.api.datastore.Query" %>
<%@ page import="com.google.appengine.api.datastore.PreparedQuery" %>
<%@ page import="com.google.appengine.api.datastore.Query.Filter" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterPredicate" %>
<%@ page import="com.google.appengine.api.datastore.Query.FilterOperator" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilter" %>
<%@ page import="com.google.appengine.api.datastore.Query.CompositeFilterOperator" %>
<%@ page import="com.google.appengine.api.datastore.Entity" %>
<%@ page import="com.google.appengine.api.datastore.FetchOptions" %>
<%@ page import="com.google.appengine.api.datastore.Key" %>
<%@ page import="com.google.appengine.api.datastore.KeyFactory" %>

<html>
	<head>
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
		</style>
	</head>
	<% 
		//saving pet name and element in new session
		monster  newmonster = new monster();
		String pic = request.getParameter("pic");
		String name = request.getParameter("name");
		String race = request.getParameter("race");
		if (name.length()==0)
		{
			name = "noname";
		}
		newmonster.setElement(pic);
		newmonster.setName(name);
		session.setAttribute("monster", newmonster);
		
		//check db if name exists
		UserService userService = UserServiceFactory.getUserService();
        User user = userService.getCurrentUser();
        
        DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
        Key key = KeyFactory.createKey("pet_stats", name);
        Filter does_exist = new FilterPredicate (Entity.KEY_RESERVED_PROPERTY, FilterOperator.EQUAL, key);
        Query q = new Query("pet_stats").setFilter(does_exist);
        PreparedQuery pq = petstatstore.prepare(q);
        List<Entity> pet_stats = pq.asList(FetchOptions.Builder.withLimit(5));  
	%>
	<body>
		<h1><font face="verdana">Pet Creation: Stage 3 of 3</h1>
		<title>MonsterSim: Create Your Pet!</title>
		<%
		if (pet_stats.isEmpty()==false)
        {
        	%>
        	<p><font face="verdana">Name exists! Return to create page?</p>
        	<form name="input" action="create.jsp" method="post">
				<input type="submit" name = "back" value="Return to homepage">
			</form>
			<%
        }
		else
		{
        %>
		<h2><font face="verdana">Please Assign Your Pet's Stats for a total of 10!</h2>
		<form name="input" action="" method="post">
			<input type="number" id=hp name="hp" min="1" max="10" value ="0"><font face="verdana"> HEALTH<br>
			<input type="number" id=atk name="atk" min="1" max="10"value ="0"><font face="verdana"> ATTACK<br>
			<input type="number" id=def name="def" min="1" max="10"value ="0"><font face="verdana"> DEFENCE<br>
			<input type="number" id=spd name="spd" min="1" max="10"value ="0"><font face="verdana"> SPEED<br><br>
			<input type = "hidden" id=type name ="race" value ="<%=race%>">
			<button type="button" onclick="myFunction()">Check Values</button><br>
			<p id="check"></p>
			<p id="check2"></p>
		</form>
		
		<form name="input" action="pet_stats.jsp" method="post">
			<input type="submit" name = "back" value="Return to homepage">
		</form>
		<%} %>
	</body>
	<script>
		function myFunction()
		{
			remain =10;
			hp = document.getElementById("hp").value;
			atk = document.getElementById("atk").value;
			def = document.getElementById("def").value;
			spd = document.getElementById("spd").value;
			type = document.getElementById("type").value;
			remain = remain-hp;
			remain = remain-atk;
			remain = remain-def;
			remain = remain-spd;
			var x="";
			if (remain>0) 
			{
				document.getElementById("check").innerHTML="Add "+remain;
			}
			else if (remain<0) 
			{
				document.getElementById("check").innerHTML="Remove "+remain;
			}
			else if (hp<=0 || atk<=0 ||def<=0||spd<=0)
			{
				document.getElementById("check").innerHTML="Negative stat!";
			}
			else if (remain == 0) 
			{
				document.getElementById("check").innerHTML="Done! Redirecting in 3 sec";
				var form = document.createElement("form");
				var inputhp = document.createElement("input");
				var inputatk = document.createElement("input");
				var inputdef = document.createElement("input");
				var inputspd = document.createElement("input");
				var raceval = document.createElement("input");
				
				form.name = "input";
				form.method = "post";
				form.action = "petConfirm.jsp";
				
				inputhp.type = "number";
				inputhp.name = "hp";
				inputhp.value = hp;
			    form.appendChild(inputhp);
			    
			    inputatk.type = "number";
				inputatk.name = "atk";
				inputatk.value = atk;
			    form.appendChild(inputatk);
			    
			    inputdef.type = "number";
				inputdef.name = "def";
				inputdef.value = def;
			    form.appendChild(inputdef);	
			    
			    inputspd.type = "number";
				inputspd.name = "spd";
				inputspd.value = spd;
			    form.appendChild(inputspd);

			    raceval.type = "text";
			    raceval.name = "type";
			    raceval.value = type;
			    form.appendChild(raceval);
			    
			    //document.getElementById("check2").appendChild(form);
			    var delay=3000//3 seconds
		    	setTimeout(function(){
					form.submit();
		    	},delay)
			}
		}
		</script>
</html>
			