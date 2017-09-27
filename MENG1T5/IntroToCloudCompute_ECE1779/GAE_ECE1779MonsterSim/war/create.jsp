<%@ page import="ece1779.monstersim.monster" %>
<%@ page import="java.util.*" %>
<%@ page import="java.util.*" %>
<%@ page import="javax.servlet.*"%>
<%@ page import="javax.servlet.jsp.*"%>
<%@ page import="javax.servlet.http.*" %>

<html>
	<head>
		<style>
		h1 {color:red;}
		h2 {color:white;}
		h3 {color:Black;}
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
		
		label img {
        
        vertical-align:middle;

    	}
		
		form {
    		display: inline;
		}
		</style>
	</head>
	<body>
		<h1><font face="verdana">Pet Creation: Stage 1 of 3</h1>
		<title>MonsterSim: Create Your Pet!</title>
		<h3><font face="verdana">Please select one of the following species of pets:</h3>
		<form name="input" action="pickAvatar.jsp" method="post">
			<br>
			<label>
      			<input id="fb1" type="radio" name="race" value="water" checked='checked'/>
      			<img src="icons/water/water-64.ico">
      		</label><br><br>
  			<label>
      			<input id="fb2" type="radio" name="race" value="fire" />
      			<img src="icons/fire/flame-64.ico">
  			</label><br><br>
  			<label>
      			<input id="fb3" type="radio" name="race" value="earth"/>
      			<img src="icons/rock/rock-64.ico">
  			</label><br><br><br>
			<input type="submit" value="Submit">
		</form>
		<form name="input" action="pet_stats.jsp" method="post">
			<input type="submit" name = "back" value="Return to homepage">
		</form>
	</body>
</html>