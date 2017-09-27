import processing.serial.*;

int lf = 10;    // Linefeed in ASCII
int comma = 44;
String myString = null;
int distance = 0;
float speed = 0;
float time= 0;





// The serial port:
Serial myPort;       

// List all the available serial ports:
//println(Serial.list());

/*  I know that the first port in the serial list on my mac
is always my  Keyspan adaptor, so I open Serial.list()[0].
Open whatever port is the one you're using.
*/

  
// Send a capital A out the serial port:
//myPort.write(65);



color fillVal1 = color(0);
color fillVal2 = color(0);
color fillVal3 = color(0);
color fillVal4 = color(0);

color fillVal_a = color(0);
color fillVal_s = color(0);

int EDGE = 500;
int BOX_SIZE = 50;


void setup () {
  
  size(EDGE,EDGE);
  myPort = new Serial(this, "COM1", 9600);
  myPort.clear();
  myString = myPort.readStringUntil(lf);
  myString = null;
  
  
}

void draw() {
  
  background(255);
  //keypress debug
  
  while (myPort.available() > 0) {
    //for (int i = 0; i < 3; i = i+1) {
      myString = myPort.readStringUntil(lf);
      if (myString != null) {
        speed = float(myString);
        
        
        
        //PFont font;
        // The font must be located in the sketch's 
        // "data" directory to load successfully
        //font = loadFont("TimesNewRomanPSMT-48.vlw"); 
        //textFont(font); 
       // fill(0, 102, 153);
        text("Speed:", 210, 20);
        println(speed);
        text (speed, 250,20);
        
      //}
    }
  }
  
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
  fill(0);
  
}
void keyPressed() {
  if (key == CODED) {
    if (keyCode == UP) {
      fillVal1 = 255;
      myPort.write(105);
    } 
    else if (keyCode == DOWN) {
      fillVal2 = 255;
      myPort.write(107);
      
    } 
    else if (keyCode == LEFT) {
      fillVal3 = 255;
      myPort.write(106);

    } 
    else if (keyCode == RIGHT) {
      fillVal4 = 255;
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
      fillVal1=0;
      myPort.write(120);
    }
    else if (keyCode == DOWN) {
      fillVal2=0;
      myPort.write(120);
    }
    else if (keyCode == LEFT) {
      fillVal3=0;
      //sends SPACE to reset position
      myPort.write(32);
    }
    else if (keyCode == RIGHT) {
      fillVal4=0;
      //sends SPACE to reset position
      myPort.write(32);
    }
  
  }
}


void keyTyped() { 
  if (key == 'a') {
    
      if (fillVal_a == 0) { fillVal_a = 255;}
      else {fillVal_a = 0;}
      
      myPort.write(97);
      //fillVal_a = 0;
  }
  if (key == 's') {
      if (fillVal_s == 0) { fillVal_s = 255;}
      else {fillVal_s = 0;}
      
      myPort.write(115);
  }
}
