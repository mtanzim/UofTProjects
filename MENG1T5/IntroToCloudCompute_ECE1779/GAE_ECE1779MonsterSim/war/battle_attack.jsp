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
	<%!//Function to send health to DB
	public int set_health (String pet_name, int damage, int pet_hp)
	{
		pet_hp = pet_hp - damage;//damaged
		if (pet_hp < 0)
		{
			pet_hp=0;
		}
		DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
	    Filter pet_only = new FilterPredicate ("pet_name", FilterOperator.EQUAL, pet_name);
	    Query q_b = new Query("pet_stats");
	    q_b.setFilter(pet_only);
		PreparedQuery pq_b = petstatstore.prepare(q_b);
	    List<Entity> pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(1));
		if (pet_stats.isEmpty())
		{
    	}
    	else 
    	{
   			for (Entity pet_stat: pet_stats) 
   			{
   				pet_stat.setProperty("cur_health", pet_hp);
				petstatstore.put(pet_stats);
			}
    	}
		return pet_hp;
	}
	%>
	<%!//Function to send health to DB
	public int set_attacker (String user_pet_name, String opp_pet_name, String nextattacker, int won)
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
   				if (won == 1)
   				{
   					pet_stat.setProperty("nextattacker", "done");
   					if (pet_stat.getProperty("pet_1").toString().equals(user_pet_name))
   					{
   						pet_stat.setProperty("pet_1", "");
   					}
   					else
   					{
   						pet_stat.setProperty("pet_2", "");
   					}
   					//Key mykey = pet_stat.getKey();
   					petstatstore.put(pet_stats);
   					//petstatstore.delete(pet_stat.getKey()); 
   					//commented out deleting immediately b/c edge cause where 
   					//first attacker is them , and you is on the "battle start" page of battle_start (first turn) and them wins and deletes db
   					//if you refreshes the page the battle will reinitialize in the DB and infintie battle, assume refreshing page is a valid option
   					//will delete battle if nextattacker is "done" in battle_search page
   					//opp_name for both monsters is already deleted in remove_opp function below so battle_search should show that monster is not in a battle
					//.put(pet_stats);
					result = 3;
   				}
   				else
   				{
   					pet_stat.setProperty("nextattacker", nextattacker);
					petstatstore.put(pet_stats);
					result = 2;
   				}
			}
    	}
		return result;
	}
	%>
	<%!//Function to remove the opp_name field in DB, so that pet can battel someone again, Updates the win loss too
	public int remove_opp (String user_pet_name, String opp_pet_name, int opp_lvl, int winner, double weakness)
	{
		String currentName="";
		int lvlup = 0;
		int totalxp=0;
		int currentxp=0;
		int oldlvl=0;
		int currlvl=0;
		int battlestat=0;
		DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
	    Filter pet_1_filter = new FilterPredicate ("pet_name", FilterOperator.EQUAL, user_pet_name);
	    Filter pet_2_filter = new FilterPredicate ("pet_name", FilterOperator.EQUAL, opp_pet_name);
	    Filter combined_filter = CompositeFilterOperator.or(pet_1_filter, pet_2_filter);
	    Query q_b = new Query("pet_stats");
	    q_b.setFilter(combined_filter);
		PreparedQuery pq_b = petstatstore.prepare(q_b);
	    List<Entity> pet_stats = pq_b.asList(FetchOptions.Builder.withLimit(5));
		if (pet_stats.isEmpty())
		{
    	}
    	else 
    	{
   			for (Entity pet_stat: pet_stats) 
   			{
   					currentName = pet_stat.getProperty("pet_name").toString();
   					if (currentName.equals(user_pet_name) & winner==1) //if pet_1 won
   					{
   						battlestat = Integer.parseInt(pet_stat.getProperty("bat_won").toString());
   						oldlvl = Integer.parseInt(pet_stat.getProperty("pet_level").toString());
   						pet_stat.setProperty("bat_won", battlestat+1);
   						//calculate XP for winner
   						currentxp = Integer.parseInt(pet_stat.getProperty("cur_exp").toString());
   						currentxp = currentxp + (int)(opp_lvl*4*(1/weakness));
   						//new (or same) level calculation, update stats if needed
   						currlvl	= (int)Math.pow(currentxp, 1.0/3) + 1;
   						if (currlvl>oldlvl)
   						{//if lvl up update atk def spd by number of lvls up
   							pet_stat.setProperty("attack_pt",Integer.parseInt(pet_stat.getProperty("attack_pt").toString())+(currlvl-oldlvl));
   							pet_stat.setProperty("defense_pt",Integer.parseInt(pet_stat.getProperty("defense_pt").toString())+(currlvl-oldlvl));
   							pet_stat.setProperty("speed_pt",Integer.parseInt(pet_stat.getProperty("speed_pt").toString())+(currlvl-oldlvl));
   							lvlup = currlvl;
   						}
   						//experience to get to next level
   						totalxp = currlvl*currlvl*currlvl;
   						//set all the values in table (loser gets nothing!)
   						pet_stat.setProperty("cur_exp", currentxp);
   						pet_stat.setProperty("max_exp", totalxp);
   						pet_stat.setProperty("pet_level", currlvl);
   								
   					}
   					else if (currentName.equals(user_pet_name) & winner==2)//if pet_1 lost
   					{//never come here since person calling this fcn is the obv winner
   						battlestat = Integer.parseInt(pet_stat.getProperty("bat_lost").toString());
   						pet_stat.setProperty("bat_lost", battlestat+1);
   					}
   					else if (currentName.equals(opp_pet_name) & winner==1)//if pet_2 lost
   					{
   						battlestat = Integer.parseInt(pet_stat.getProperty("bat_lost").toString());
   						pet_stat.setProperty("bat_lost", battlestat+1);
   					}
   					else if (currentName.equals(opp_pet_name) & winner==2)// if pet_2 won
   					{//never come here since person calling this fcn is the obv winner
   						battlestat = Integer.parseInt(pet_stat.getProperty("bat_won").toString());
   						pet_stat.setProperty("bat_won", battlestat+1);
   					}
   					pet_stat.setProperty("opp_name", "");
   					petstatstore.put(pet_stat);
			}
    	}
		return lvlup;
	}
	%>
  	<body>
  	
  		<% 
  		//get all values needed from battle_start
  		String whoattacked = request.getParameter("whoattack");
  		String whoareyou = request.getParameter("whoareyou");
  		String attacktype = request.getParameter("attacktype");
  		
  		String user_speed = request.getParameter("user_speed");//storing for battle user stats
  		int user_atk = Integer.parseInt(request.getParameter("user_atk"));
  		int user_def = Integer.parseInt(request.getParameter("user_def"));
  		String user_type = request.getParameter("user_type");
  		int user_lvl = Integer.parseInt(request.getParameter("user_lvl"));
  		int user_hp = Integer.parseInt(request.getParameter("user_hp"));
  		String user_name = request.getParameter("user");

  		int opp_lvl = Integer.parseInt(request.getParameter("opp_lvl"));//storing for battle opp stats
  		String opp_type = request.getParameter("opp_type");
  		int opp_hp = Integer.parseInt(request.getParameter("opp_hp"));
  		int opp_speed = Integer.parseInt(request.getParameter("opp_speed"));
  		int opp_atk = Integer.parseInt(request.getParameter("opp_atk"));
  		int opp_def = Integer.parseInt(request.getParameter("opp_def"));
  		String opp_name = request.getParameter("opponent");
  		
  		%>
  		<h1><font face="verdana"> YOUR PET ATTACKED </h1>
  		
		<%
		String nextattacker = "nobody";
		//random number for damage calc
		int lowrand = 70;
		int highrand = 100;
		double rand = (Math.random() * (highrand - lowrand)) + lowrand;
		//random number for attack accuracy
		int lowacc = 1;
		int highacc = 10;
		double randacc = (Math.random() * (highacc - lowacc)) + lowacc;
		
		int attackresult=0;
		//damage modifier
		double weakness = 1;
		//new hp
		int new_hp=0;
		if (whoattacked.isEmpty())
		{
			whoattacked="";//no whoattacked?
			%><h1><font face="verdana"> <%=whoattacked %> nobody attacked? Refresh page</h1><%
		}
		
		//YOU ATTACK
		%><p> <img alt="" src="icons/<%= user_type%>breath.gif";" /></p><%
		//before battle, calculate damage modifier
		if (user_type.contains("water")) {
			if (opp_type.contains("fire"))
				weakness = 1.5;
		}
		else if (user_type.contains("fire")){
			if (opp_type.contains("earth"))
				weakness = 1.5;
		}
		else {
			if (opp_type.contains("water"))
				weakness = 1.5;
		}
		//calculate damage
		double damage = (((((2*user_lvl/5)+2)*user_atk/opp_def)/50)+2)*weakness*rand/100;
		//int damageint = (int) Math.round(damage); do this later
		if (attacktype.equals("strong"))
		{
			%><font face="verdana">STRONG ATTACK 
<%-- 			<br>randacc = <%=randacc%> <br> --%>
			<%
			//int cointoss=(Math.random()<0.5)?0:1;// 50/50 random
			if (randacc<4)//%40 chance of hit
			{
				%><font face="verdana">HIT <br><%
				damage=damage*2;
			}
			else
			{
				%><font face="verdana">MISSED <br><%
				damage=0;
			}
		}
		else if (attacktype.equals("weak")) 
		{
			%><font face="verdana">WEAK ATTACK 
<%-- 			<br>randacc = <%=randacc%> --%>
			<%
			if (randacc<11)//always hits!
			{
				%><font face="verdana">HIT <br><%
				damage=damage/2;
			}
			else
			{
				%><font face="verdana">MISSED <br><%
				damage=0;
			}
		}
		else 
		{
			%><font face="verdana">ATTACK 
<%-- 			<br>randacc = <%=randacc%> --%>
			<%
			if (randacc<7)//%70 chance of hit
			{
				%><font face="verdana">HIT <br><%
				//damageint=damageint*2;
			}
			else
			{
				%><font face="verdana">MISSED <br><%
				damage=0;
			}
		}
		int damageint = (int) Math.round(damage);
		%>
<%-- 			RAW stats <%= damage%> damage! weakness = <%=weakness %> rand = <%=rand %><br> --%>
			<h2><font face="verdana"><%=user_name %> DID <%=damageint %> DAMAGE</h2>
		<%
		//database update with remaining hp of opponent
		new_hp=set_health(opp_name,damageint,opp_hp);//put new health in DB
		%>
		<h2><font face="verdana">Opponent's new hp is <%= new_hp%> </h2><br>
		<%
		
		
		if (whoareyou.equals("you")) // you started the battle, you are "you"
		{
			nextattacker="them";
		}
		else if (whoareyou.equals("them"))//you aren't the battle initiator, you are "them"
		{
			nextattacker="you";
		}
		
		int winner = 0;
		if (new_hp > 0)
		{
			attackresult = set_attacker(user_name,opp_name,nextattacker,0);
			if (attackresult == 1)
			{
				%><font face="verdana">empty attack! <%
			}
			else if (attackresult == 2)
			{
				%><font face="verdana">
<%-- 				<%=nextattacker %> are attacking next!  --%>
				OPPONENT'S TURN
				<%
			}	
  		%>
  			
		<form action="battle_start.jsp" method="post">
			<h2> 
			<input name="whoattack" type="hidden" value="<%=nextattacker%>" />
			<input name="nextattacker" type="hidden" value="<%=nextattacker%>" />
			<input name="opponent" type="hidden" value=<%=opp_name %> />
			<input name="user" type="hidden" value=<%=user_name %> />
			<input name="create" type="submit" value="Next Turn" /></h2>
		</form>
		<%
		}
		else
		{//battle over!
			if (whoareyou.equals("you")) // you started the battle, you are "you"
			{
				winner=1;
			}
			else if (whoareyou.equals("them"))//you aren't the battle initiator, you are "them"
			{
				winner=2;
			}
			winner = 1;//obv the person that reaches here will be hte winner since he set the opponents hp to 0 duhhh
			set_attacker(user_name,opp_name,nextattacker,1);
			int lvlup = remove_opp(user_name,opp_name,opp_lvl,winner, weakness);
			if (lvlup>0)
			{
				%>
				<%=user_name %> IS NOW LEVEL <%=lvlup %>! <br>
				<% 
			}
		%>
			<h2><font face="verdana">Your Pet <%=user_name %> won!</h2> <br>Battle deleted from db!
		<%
		}%>
		<form action="pet_stats.jsp" method="post">
			<h2> <input name="return" type="submit" value="Return to Main Page" /></h2>
		</form>

	</body>
</html>