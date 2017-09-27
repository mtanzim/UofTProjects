package ece1779;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class Logout extends HttpServlet {
	public void doGet(HttpServletRequest request,HttpServletResponse response)
	{
		request.getSession().invalidate();
		//request.getSession().setAttribute("loggedIn", null);	
		try {
				response.sendRedirect(request.getContextPath() + "/src/userUI.jsp");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		
	}
}