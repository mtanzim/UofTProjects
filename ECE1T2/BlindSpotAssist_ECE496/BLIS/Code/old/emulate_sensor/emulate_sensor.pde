import processing.serial.*;

// The serial port:
Serial myPort;      

void setup () {
  
  //emulated port
  myPort = new Serial(this, "COM1", 9600);
  
}

void draw() {
  
  float r = random(45, 50);
  println(r);
  myPort.write(str(r));
  myPort.write(10);
//  myPort.write("start");
//  myPort.write(10);
//  myPort.write("14");
//  myPort.write(10);
//  myPort.write("15.345");
//  myPort.write(10);
//  myPort.write("16");
//  myPort.write(10);
}
