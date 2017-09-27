import processing.serial.*;
import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.traces.ILine2DEquation;
import org.gwoptics.graphics.graph2D.traces.RollingLine2DTrace;

int lf = 10;    // Linefeed in ASCII
int comma = 44;
String myString = null;
int distance = 0;
float speed = 0;
float time= 0;



int fill_off=0;
int fill_on=255;

color fillVal1 = color(fill_off);
color fillVal2 = color(fill_off);
color fillVal3 = color(fill_off);
color fillVal4 = color(fill_off);

color fillVal_a = color(fill_off);
color fillVal_s = color(fill_off);



int EDGE = 725;
int GRAPH_X= 600;
int GRAPH_Y=500;
int BOX_SIZE = 50;

// The serial port:
Serial myPort;       


//graph class definition; DEFINE THE DATA HERE
class eq implements ILine2DEquation{
	public double computePoint(double x,int pos) {
		return speed;


    	  }		
}

//declare graph classes
RollingLine2DTrace r;
Graph2D g;

void setup () {
  
  
  size(EDGE,EDGE);
  
  //COM Port Setup
  myPort = new Serial(this, "COM1", 9600);
  myPort.clear();
  myString = myPort.readStringUntil(lf);
  myString = null;
  
  
  //graph setup
  r  = new RollingLine2DTrace(new eq() ,100,0.1f);
  r.setTraceColour(255, 0, 0);
  
  g = new Graph2D(this, GRAPH_X, GRAPH_Y, false);
 
  //trace data
  g.addTrace(r);
  g.setYAxisMax(200);
  g.setXAxisLabel("Sample Point");
  g.setYAxisLabel("Speed [cm/s]");
  g.position.y = 50;
  g.position.x = 100;
  g.setYAxisTickSpacing(10);
  g.setXAxisMax(10f);
  
  
  
  
  
}

void draw() {
  
  background(255);
  //keypress debug
  get_serial_data();
  println(speed);
  text ("Speed:", EDGE - 115, 25);
  text (speed,EDGE - 65 ,25);
  
  text ("Distance:", EDGE - 115, 15);
  text (speed, EDGE - 65 ,15);
  
  text ("Time:", EDGE - 115, 35);
  text (speed, EDGE - 65 ,35);
  
  
  //up box
  fill(fillVal1);
  rect(EDGE-2*BOX_SIZE, EDGE-2*BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //down box  
  fill(fillVal2);
  rect(EDGE-2*BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //left box
  fill(fillVal3);
  rect(EDGE-3*BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //right box
  fill(fillVal4);
  rect(EDGE-BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //a box
  fill(fillVal_a);
  rect(0, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
  //s box
  fill(fillVal_s);
  rect(BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE); 


  //reset fill back to black
  fill(fill_off);
  
  g.draw();
  
}

//serial data function
void get_serial_data () {
  

            while (myPort.available() > 0) {
              myString = myPort.readStringUntil(lf);
              if (myString != null) {
                speed = float(myString);
              }
            }
            
            //return speed;
}



//keyboard functions

void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      fillVal1 = fill_on;
      myPort.write(105);
    } 
    else if (keyCode == DOWN) {
      fillVal2 = fill_on;
      myPort.write(107);
      
    } 
    else if (keyCode == LEFT) {
      fillVal3 = fill_on;
      myPort.write(106);

    } 
    else if (keyCode == RIGHT) {
      fillVal4 = fill_on;
      myPort.write(108); 
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
      myPort.write(120);
    }
    else if (keyCode == DOWN) {
      fillVal2=fill_off;
      myPort.write(120);
    }
    else if (keyCode == LEFT) {
      fillVal3=fill_off;
      //sends SPACE to reset position
      myPort.write(32);
    }
    else if (keyCode == RIGHT) {
      fillVal4=fill_off;
      //sends SPACE to reset position
      myPort.write(32);
    }
  
  }
}


void keyTyped() { 
  if (key == 'a') {
    
      if (fillVal_a == fill_off) { fillVal_a = fill_on;}
      else {fillVal_a = fill_off;}
      
      myPort.write(97);
      //fillVal_a = 0;
  }
  if (key == 's') {
      if (fillVal_s == fill_off) { fillVal_s = fill_on;}
      else {fillVal_s = fill_off;}
      
      myPort.write(115);
  }
}
