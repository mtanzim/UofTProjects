package ece1779.monstersim;

import ece1779.monstersim.monster;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.http.*;

import java.util.*;
 
public class Initialization extends HttpServlet {
    public void init(ServletConfig config) {
    	try {
		    //Initialize Monster
    		//new monster = monster();
    		monster newmonster = new monster();
    		
		}
		catch (Exception ex) {
		    getServletContext().log("Error: " + ex.getMessage());
		}
    }
}