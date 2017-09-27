import processing.serial.*;
import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.traces.ILine2DEquation;
import org.gwoptics.graphics.graph2D.traces.RollingLine2DTrace;

PFont fontA;

String COM_PORT="COM8";


int lf = 10;    // Linefeed in ASCII
int comma = 44;
String myString = null;
float distance = 0;
float speed = 0;
float time= 0;

int s_level_MAX=8; //change this accroding to slave's pwm levels
int s_level=s_level_MAX;
float current_pwm=0.0;
float danger_level=0.0;
int danger_box_color=0;
float sharp_distance=0.0;

Boolean first_up=true;
Boolean first_down=true;
Boolean first_left=true;
Boolean first_right=true;



int fill_off=0;
int fill_on=255;

color fillVal1 = color(fill_off);
color fillVal2 = color(fill_off);
color fillVal3 = color(fill_off);
color fillVal4 = color(fill_off);

color fillVal_a = color(fill_off);
color fillVal_s = color(fill_off);



int EDGE = 770;
int h_offset=340;

//graph 1 (speed) parameters
int GRAPH_Y=350;
int GRAPH_X= 450;
int GRAPH_YST=50;
int GRAPH_XST=75;

//graph 2 (distance) parameters
int GRAPH_Y2=350;
int GRAPH_X2= 450;
int GRAPH_YST2=50;
int GRAPH_XST2=610;

//graph 3 (time) parameters
int GRAPH_Y3=200;
int GRAPH_X3= 450;
int GRAPH_YST3=450;
int GRAPH_XST3=75;

//information box
int INFO_BOX_SIZE_X=450;
int INFO_BOX_SIZE_Y=200;
int INFO_BOX_Y=450;
int INFO_BOX_X=610;
//int INFO_BOX_X=EDGE-50, 
//int INFO_BOX_Y=EDGE-50;



int BOX_SIZE = 50;

// The serial port:
Serial myPort;       


//graph class definition; DEFINE THE DATA HERE
class eq implements ILine2DEquation{
	public double computePoint(double x,int pos) {
		return speed;


    	  }		
}

class eq2 implements ILine2DEquation{
	public double computePoint(double x,int pos) {
		return distance;


    	  }		
}

class eq3 implements ILine2DEquation{
	public double computePoint(double x,int pos) {
		return time;


    	  }		
}

//declare graph classes
RollingLine2DTrace r,r2,r3;
Graph2D g,g2,g3;

void setup () {
  
  
  size(EDGE+h_offset,EDGE);
  
  
  //font setup
  fontA = loadFont("Arial-BoldMT-48.vlw");
  textFont(fontA, 12);
  //COM Port Setup
  myPort = new Serial(this, COM_PORT, 9600);
  myPort.clear();
  myString = myPort.readStringUntil(lf);
  myString = null;
  
  
  //graph setup
  r  = new RollingLine2DTrace(new eq() ,100,0.1f);
  r.setTraceColour(255, 0, 0);
  
  r2=new RollingLine2DTrace(new eq2() ,100,0.1f);
  r2.setTraceColour(0, 255, 0);
  
  r3=new RollingLine2DTrace(new eq3() ,100,0.1f);
  r3.setTraceColour(0, 0, 255);
  
  //speed graph
  g = new Graph2D(this, GRAPH_X, GRAPH_Y, false);
  //distance grpah
  g2 = new Graph2D(this, GRAPH_X2, GRAPH_Y2, false);
  //time graph
  g3 = new Graph2D(this, GRAPH_X3, GRAPH_Y3, false);
 
  //trace data
  g.addTrace(r);
  //g.addTrace(r2);
  g.setYAxisMax(100);
  g.setYAxisMin(-150);
  g.setXAxisLabel("Sample Point");
  g.setYAxisLabel("Speed [cm/s]");
  g.position.y = GRAPH_YST;
  g.position.x = GRAPH_XST;
  g.setYAxisTickSpacing(10);
  g.setXAxisMax(8f);
  
  
  //definitions for distance graph
  g2.addTrace(r2);
  //g.addTrace(r2);
  g2.setYAxisMax(350);
  g2.setYAxisMin(0);
  g2.setXAxisLabel("Sample Point");
  g2.setYAxisLabel("Distance [cm]");
  g2.position.y = GRAPH_YST2;
  g2.position.x = GRAPH_XST2;
  g2.setYAxisTickSpacing(10);
  g2.setXAxisMax(8f);
  
  
  //definitions for time graph
  g3.addTrace(r3);
  //g.addTrace(r2);
  g3.setYAxisMax(10);
  g3.setYAxisMin(0);
  g3.setXAxisLabel("Sample Point");
  g3.setYAxisLabel("Time Remaining [s]");
  g3.position.y = GRAPH_YST3;
  g3.position.x = GRAPH_XST3;
  g3.setYAxisTickSpacing(2.5);
  g3.setXAxisMax(8f);
  
}

void draw() {
  
  background(255);
  //keypress debug
  get_serial_data();
  //println(speed);

  
  
  //up box
  fill(fillVal1);
  rect(h_offset+EDGE-2*BOX_SIZE, EDGE-2*BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //down box  
  fill(fillVal2);
  rect(h_offset+EDGE-2*BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //left box
  
  if (danger_level < 4.0){
    
    fill(fillVal3);
    rect(h_offset+EDGE-3*BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  }
  else
  {
    fill(255,0,0);
    rect(h_offset+EDGE-3*BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  }
  
  

  
  
    
  
  //right box
  fill(fillVal4);
  rect(h_offset+EDGE-BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //a box
  fill(fillVal_a);
  rect(0, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //s box
  fill(fillVal_s);
  rect(BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE); 
  
  
  //information box
  fill (255);
  rect (INFO_BOX_X, INFO_BOX_Y,INFO_BOX_SIZE_X, INFO_BOX_SIZE_Y);
  


  
  
  
    
  //danger display box
  
  int text_offset_x=20;
  int text_offset_y=18;
  int box_offset_x=20;
  int box_offset_y=25;
  
  if (danger_level == 0.0){fill(0,254,0);text ("No Danger", INFO_BOX_X+text_offset_x, INFO_BOX_Y+text_offset_y);}
  else if (danger_level == 1.0){fill(125,125,0);text ("Obstacle Approaching", INFO_BOX_X+text_offset_x, INFO_BOX_Y+text_offset_y);}
  else if (danger_level == 2.0){fill(200,200,0);text ("Obstacle Approaching", INFO_BOX_X+text_offset_x, INFO_BOX_Y+text_offset_y);}
  else if (danger_level == 3.0){fill(254,254,0);text ("Obstacle Approaching", INFO_BOX_X+text_offset_x, INFO_BOX_Y+text_offset_y);}
  else if (danger_level == 4.0){fill(254,0,0);text ("Caution, Steering Locked", INFO_BOX_X+text_offset_x, INFO_BOX_Y+text_offset_y);}
  else if (danger_level == 5.0){fill(254,0,0);text ("Danger, Steering Locked", INFO_BOX_X+text_offset_x, INFO_BOX_Y+text_offset_y);}
  
 
    
  rect (INFO_BOX_X+box_offset_x, INFO_BOX_Y+box_offset_y,BOX_SIZE, BOX_SIZE);

  
  
  
  
  //reset fill back to black
  fill(fill_off);
  
  int info_offset_x=20;
  int info_offset_y=90;
  int info_value_offset=85;
  int vert_spacing = 10;
  
  text ("Distance: ", INFO_BOX_X+info_offset_x, INFO_BOX_Y+info_offset_y);
  text (distance,INFO_BOX_X+info_offset_x+info_value_offset, INFO_BOX_Y+info_offset_y);
  
  text ("Speed:", INFO_BOX_X+info_offset_x, INFO_BOX_Y+info_offset_y+vert_spacing);
  text (speed, INFO_BOX_X+info_offset_x+info_value_offset, INFO_BOX_Y+info_offset_y+vert_spacing);
  
  text ("Time:", INFO_BOX_X+info_offset_x, INFO_BOX_Y+info_offset_y+2*vert_spacing);
  text (time, INFO_BOX_X+info_offset_x+info_value_offset, INFO_BOX_Y+info_offset_y+2*vert_spacing);
  
  text ("Danger Level:", INFO_BOX_X+info_offset_x, INFO_BOX_Y+info_offset_y+3*vert_spacing);
  text (danger_level, INFO_BOX_X+info_offset_x+info_value_offset, INFO_BOX_Y+info_offset_y+3*vert_spacing);
  
  text ("PWM LEVEL: ", 5, EDGE - 65);
  //text (s_level, 5+75, EDGE - 65);
  text (current_pwm/254, 5+75, EDGE - 65);
  
  
  
  
  
  
  
  g.draw();
  g2.draw();
  g3.draw();
  
}

//serial data function
void get_serial_data () {
  

            while (myPort.available() > 0) {
              myString = myPort.readStringUntil(lf);
              //myString = myPort.readString();
              if (myString != null) {
                //text (myString,EDGE - 500 ,25);
                //speed = float(myString);
                
                //String[] p = splitTokens("a,b,c,d,e\n", ",");
                
                
                
                String [] p = splitTokens(myString, ",");
                

                
                //for (int i=0; i<3; i++) {
                  //if (p[i] != null) {
                    if (p.length == 6) {  
                      distance = float(p[0]);
                      speed = float(p[1]);
                      time= float (p[2]);
                      current_pwm= float (p[3]);
                      danger_level= float (p[4]);
                      sharp_distance= float (p[5]);
                    }
                    
                    //text (distance, EDGE - 500 ,25+10); // Prints "a"
                    //text (speed, EDGE - 500 ,25+20); // Prints "b"
                    //text (time, EDGE - 500 ,25+30); // Prints "c"
                  //}
                //}
                
                //println(p[2]); // Prints "c"
   
                
              }
            }
            
            //return speed;
}



//keyboard functions

void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      fillVal1 = fill_on;
      
      
      //need to only write once
      if (first_up) {
        myPort.write(105);
        first_up=false;
      }
 
    } 
    else if (keyCode == DOWN) {
      fillVal2 = fill_on;
      
      //only write once
      if (first_down) {
        myPort.write(107);
        first_down=false;
      }

      
      
    } 
    else if (keyCode == LEFT) {
      fillVal3 = fill_on;
      
      //only write once
      if (first_left) {
        //don't allow collision
        if (danger_level <4.0){
          myPort.write(106);
          first_left=false;
        }
        
      }

    } 
    else if (keyCode == RIGHT) {
      fillVal4 = fill_on;
      
      //only write once
      if (first_right) {
        myPort.write(108); 
        first_right=false;
      }
    }
    else {
    //fillVal1 = 0;
    //fillVal2 = 0;
    //myPort.write(120);
    }
  }
}

void keyReleased() {
  if (key == CODED) {
    if (keyCode == UP) {
      fillVal1=fill_off;
      
      
      myPort.write(107);
      myPort.write(120);
      first_up=true; //reset first_x

      //delay (100);
      //myPort.write(120);
    }
    else if (keyCode == DOWN) {
      fillVal2=fill_off;
      myPort.write(105);
      myPort.write(120);
      first_down=true; //reset first_x

      //delay (100);
      //myPort.write(120);
    }
    else if (keyCode == LEFT) {
      fillVal3=fill_off;
      //sends SPACE to reset position
      myPort.write(32);
      first_left=true; //reset first_x
    }
    else if (keyCode == RIGHT) {
      fillVal4=fill_off;
      //sends SPACE to reset position
      myPort.write(32);
      first_right=true; //reset first_x
    }
  
  }
}


void keyTyped() { 
  if (key == 'a') {
    
      if (fillVal_a == fill_off) { fillVal_a = fill_on;}
      else {fillVal_a = fill_off;}
      
      myPort.write(97);
      if (s_level != s_level_MAX) {
        s_level ++;
      }
      //fillVal_a = 0;
  }
  if (key == 's') {
      if (fillVal_s == fill_off) { fillVal_s = fill_on;}
      else {fillVal_s = fill_off;}
      
      myPort.write(115);
      
      if (s_level != 1) {
        s_level --;
      }
      
  }
}
