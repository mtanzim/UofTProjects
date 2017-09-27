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
		
		.CSSTableGenerator {
		margin:0px;padding:0px;
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
		</style>
	</head>
	<body>
		<h1><font face="verdana">Pet Creation Complete!</h1>
		<title>MonsterSim: Pet Creation Complete!</title>
		<h2><font face="verdana">Your Pet:</h2>
		<%
			//set all other attributes
			monster  newmonster = (monster)session.getAttribute("monster");
			//all the attributes for a fresh monstaa
			int hp = Integer.parseInt(request.getParameter("hp"));
			int atk = Integer.parseInt(request.getParameter("atk"));
			int def = Integer.parseInt(request.getParameter("def"));
			int spd = Integer.parseInt(request.getParameter("spd"));
			String race = request.getParameter("type");
			
			Integer pet_level = 1;
	      	Integer max_exp=1;
	      	Integer cur_exp=0;
	      	
	      	Integer cur_health=hp;
	      	Integer max_health=hp;
	      	Integer defense_pt = def;
	      	Integer attack_pt = atk;
	      	Integer speed_pt = spd;
	      	
	      	Integer bat_won=0;
	      	Integer bat_lost=0;
	      	//save locally
			newmonster.setHp(hp);
			newmonster.setAtk(atk);
			newmonster.setDef(def);
			newmonster.setSpd(spd);
			newmonster.setWin(0);
			newmonster.setLoss(0);
			session.setAttribute("monster", newmonster);
			monster  getmonster = (monster)session.getAttribute("monster");
			
			//save to db 
			UserService userService = UserServiceFactory.getUserService();
	        User user = userService.getCurrentUser();
	        
			String pet_db_name = "pet_db";
	        //Key pet_db_Key = KeyFactory.createKey("pet_db", pet_db_name);
	        Entity pet_stats = new Entity("pet_stats", getmonster.getName());
	        
	        pet_stats.setProperty("avatar_url", getmonster.getElement());
	        pet_stats.setProperty("pet_name", getmonster.getName());
	        pet_stats.setProperty("user",user.getNickname());
	        pet_stats.setProperty("pet_level",pet_level);
	      	pet_stats.setProperty("max_exp", max_exp);
	      	pet_stats.setProperty("cur_exp", cur_exp);
	      	pet_stats.setProperty("cur_health", cur_health);
	      	pet_stats.setProperty("max_health", max_health);
	      	pet_stats.setProperty("defense_pt", defense_pt);
	      	pet_stats.setProperty("attack_pt", attack_pt);
	      	pet_stats.setProperty("speed_pt", speed_pt);
	      	pet_stats.setProperty("bat_won", bat_won);
	      	pet_stats.setProperty("bat_lost", bat_lost);
	      	pet_stats.setProperty("opp_name", "");
	      	
	      	Date date = new Date();
	      	pet_stats.setProperty("date", date);
	        
	        DatastoreService petstatstore = DatastoreServiceFactory.getDatastoreService();
	        petstatstore.put(pet_stats);
	        ////////////done saving to db
	        if (race.equals("fire")) {
		   %>
		   <table class="CSSTableGenerator">
              <tr>
                  <td style="background-color:#f5ac79;">
                      <font face="verdana"><%=getmonster.getName() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#f5ac79;">
                     <img src="icons/<%=getmonster.getElement()%>.gif">
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#f5ac79;">
                     <font face="verdana">Health:  <%=getmonster.getHp() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#f5ac79;">
                     <font face="verdana">Attack:  <%=getmonster.getAtk() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#f5ac79;">
                      <font face="verdana">Defence: <%=getmonster.getDef() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#f5ac79;">
                      <font face="verdana">Speed: <%=getmonster.getSpd() %>
                  </td>
              </tr>
          </table>
          <%
	        }
	        if (race.equals("water")) {
		   %>
		   <table class="CSSTableGenerator">
              <tr >
                  <td style="background-color:#9db7f4;">
                      <font face="verdana"><%=getmonster.getName() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#9db7f4;">
                     <img src="icons/<%=getmonster.getElement()%>.gif">
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#9db7f4;">
                     <font face="verdana">Health:  <%=getmonster.getHp() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#9db7f4;">
                     <font face="verdana">Attack:  <%=getmonster.getAtk() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#9db7f4;">
                      <font face="verdana">Defence: <%=getmonster.getDef() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#9db7f4;">
                      <font face="verdana">Speed: <%=getmonster.getSpd() %>
                  </td>
              </tr>
          </table>
          <%
	        }
	        if (race.equals("earth")) {
		   %>
		   <table class="CSSTableGenerator">
              <tr>
                  <td style="background-color:#d2c17c;">
                      <font face="verdana"><%=getmonster.getName() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#d2c17c;">
                     <img src="icons/<%=getmonster.getElement()%>.gif">
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#d2c17c;">
                     <font face="verdana">Health:  <%=getmonster.getHp() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#d2c17c;">
                     <font face="verdana">Attack:  <%=getmonster.getAtk() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#d2c17c;">
                      <font face="verdana">Defence: <%=getmonster.getDef() %>
                  </td>
              </tr>
              <tr>
                  <td style="background-color:#d2c17c;">
                      <font face="verdana">Speed: <%=getmonster.getSpd() %>
                  </td>
              </tr>
          </table>
          <%
	        }
	      %>
          <br><br>
       	<form name="input" action="pet_stats.jsp" method="post">
			<input type="submit" name = "back" value="Return to homepage">
		</form>
		     
	</body>
</html>