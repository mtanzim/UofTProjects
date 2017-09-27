import processing.serial.*;

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
  myPort = new Serial(this, "COM4", 9600);
  
  
}

void draw() {
  
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
  rect(0+BOX_SIZE, EDGE-BOX_SIZE, BOX_SIZE, BOX_SIZE);
  
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
