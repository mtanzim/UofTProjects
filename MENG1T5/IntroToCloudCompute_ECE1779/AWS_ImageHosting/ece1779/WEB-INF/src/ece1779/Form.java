package ece1779;

import java.io.IOException;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.sql.DataSource;

import java.sql.*;

import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class Form extends HttpServlet {
    public void doGet(HttpServletRequest request,
	              HttpServletResponse response)
    throws IOException, ServletException
    {
    	String message = "NULL";
    	if (request.getParameter("r_pass").equals(request.getParameter("r_c_pass"))) {
	    	try {
	    		DataSource dbcp = (DataSource)this.getServletContext().getAttribute("dbpool");
				Connection con =  dbcp.getConnection();
				
				System.out.println("got connection");
				String username = request.getParameter("r_user");
				String password = request.getParameter("r_pass");
				message = "Inside-Got USERNAME";
				String sql = "SELECT login, password, id from users WHERE login = ?";
				PreparedStatement s = con.prepareStatement(sql);
				s.setString(1,username);
				ResultSet rs = s.executeQuery();
				if (rs.next()) {
					message = "username already exists, please choose a different username";
			    	request.setAttribute("userError", message);
			    	message = "FAIL";
			    	request.setAttribute("ACK", message);
				}
				else {
					//new username - insert it to the database
					String query = "INSERT INTO users (login, password) " + "VALUES ('"+username+"','"+password+"')";
					//Execute SQL query
					Statement s1 = (Statement) con.createStatement();
					s1.executeUpdate(query);
					s1.close();
					message = "username successfully added";
			    	request.setAttribute("userSucc", message);
			    	message = "PASS";
			    	request.setAttribute("ACK", message);
				}
				rs.close();
				s.close();
				con.close();
	    		
	    	} catch (SQLException e2) {
			  	// Exception when executing java.sql related commands, print error message to the console
			  	System.out.println(e2.toString());
			}
    	} 
    	else {
    		message = "PFAIL";
    		request.setAttribute("ACK", message);
    	}
		request.getRequestDispatcher("/src/RegisterPage.jsp").forward(request, response);
    }
    // Do this because the servlet uses both post and get
    public void doPost(HttpServletRequest request,
	               HttpServletResponse response)
       	throws IOException, ServletException {
    	doGet(request,response);
    }
}
