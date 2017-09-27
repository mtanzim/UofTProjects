<%@ page import="ece1779.monstersim.monster" %>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ page import="java.util.List" %>
<%@ page import="com.google.appengine.api.users.User" %>
<%@ page import="com.google.appengine.api.users.UserService" %>
<%@ page import="com.google.appengine.api.users.UserServiceFactory" %>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<html>
	<head>
		<style>
		h1 {color:red; text-align:center;}
		h2 {color:blue; text-align:center;}
		p1 {color:white; text-align:center;}
		p2 {color:red; text-align:center;}
		p3 {color:green; text-align:center;}
		</style>
	</head>
	
   <body>
	<h1><font face="verdana">MonsterSim: Create Your Pet!</h1>
    <title>MonsterSim: Create Your Pet!</title>
	
<%
   	UserService userService = UserServiceFactory.getUserService();
   	User user = userService.getCurrentUser();
   	if (user != null) {
     	pageContext.setAttribute("user", user);
     	response.sendRedirect("pet_stats.jsp");
   	} else {
%>
	<div class="center">
		<p1><font face="verdana">Sign in with your google account to create your own pet and fight with monsters to gain points!</p1>
	</div>
	</br></br>
	<div class="center">
		<a class="css_btn_class" href="<%= userService.createLoginURL(request.getRequestURI()) %>">Google Login</a>
	</div>
<%
  	}
%>

<style type="text/css">

.center { text-align: center; }

.css_btn_class {
	font-size:20px;
	font-family:Arial;
	font-weight:normal;
	-moz-border-radius:8px;
	-webkit-border-radius:8px;
	border-radius:8px;
	border:1px solid #d02718;
	padding:9px 18px;
	text-decoration:none;
	background:-moz-linear-gradient( center top, #f24537 5%, #c62d1f 100% );
	background:-ms-linear-gradient( top, #f24537 5%, #c62d1f 100% );
	filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#f24537', endColorstr='#c62d1f');
	background:-webkit-gradient( linear, left top, left bottom, color-stop(5%, #f24537), color-stop(100%, #c62d1f) );
	background-color:#f24537;
	color:#ffffff;
	display:inline-block;
	text-shadow:1px 0px 0px #810e05;
 	-webkit-box-shadow:inset 1px 3px 1px 0px #f5978e;
 	-moz-box-shadow:inset 1px 3px 1px 0px #f5978e;
 	box-shadow:inset 1px 3px 1px 0px #f5978e;
}.css_btn_class:hover {
	background:-moz-linear-gradient( center top, #c62d1f 5%, #f24537 100% );
	background:-ms-linear-gradient( top, #c62d1f 5%, #f24537 100% );
	filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#c62d1f', endColorstr='#f24537');
	background:-webkit-gradient( linear, left top, left bottom, color-stop(5%, #c62d1f), color-stop(100%, #f24537) );
	background-color:#c62d1f;
}.css_btn_class:active {
	position:relative;
	top:1px;
}

html { 
background: url(elements.jpg) no-repeat center center fixed; 

-webkit-background-size: cover; 
-moz-background-size: cover; 
-o-background-size: cover; 
background-size: cover; 
} 
</style> 
</style>
    
  </body>
</html>