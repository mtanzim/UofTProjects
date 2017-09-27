
#include <Wire.h>

const int SLAVE_ADDRESS=4;

int M1_A = 12; // H-bridge leg 1 Motor 1
int M1_PWM = 11; // PWM H-bridge enable pin for speed control
int M1_B = 10; // H-bridge leg 2 Motor 1

int M2_A = 4; // H-bridge leg 1 Motor 2
int M2_PWM = 3; // PWM H-bridge enable pin for speed control
int M2_B = 2; // H-bridge leg 2 Motor 2

int LED = 13; // LED pin attached to Arduino pin 13
int LED_PWM=6;
int incomingByte = 0; // variable to store serial data
int speed_val = 200; // variable to store speed value

//////////////////////////////////////////////////////////////////////////////////////////


void setup(){

  Wire.begin(SLAVE_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event

//Serial.begin(115200);

// set digital i/o pins as outputs:
//
  pinMode(LED, OUTPUT);
  pinMode(LED_PWM, OUTPUT);
  pinMode(M1_A, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_B, OUTPUT);
  pinMode(M2_A, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_B, OUTPUT);

}

////////////////////////////////////

void loop(){
  delay (100);

}



/////////// motor functions ////////////////

void M1_reverse(int z){
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_A, HIGH);
  analogWrite(M1_PWM, z);
}

void M1_forward(int z){
  digitalWrite(M1_A, LOW);
  digitalWrite(M1_B, HIGH);
  analogWrite(M1_PWM, z);
}

void M1_stop(){
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_A, LOW);
  digitalWrite(M1_PWM, LOW);
}

void M2_right(int y){
  digitalWrite(M2_B, LOW);
  digitalWrite(M2_A, HIGH);
  analogWrite(M2_PWM, y);
}

void M2_left(int y){
  digitalWrite(M2_A, LOW);
  digitalWrite(M2_B, HIGH);
  analogWrite(M2_PWM, y);
}

void M2_stop(){
  digitalWrite(M2_B, LOW);
  digitalWrite(M2_A, LOW);
  digitalWrite(M2_PWM, LOW);
}



void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.receive(); // receive byte as a character
    //Serial.print(c);         // print the character
  }
  int x = Wire.receive();    // receive byte as an integer
  //Serial.println(x);         // print the integer
  
  
  
  //digitalWrite(LED, HIGH); //quick debug
  //analogWrite(LED_PWM, 255);
    // if byte is equal to "105" or "i", go forward
  if (x == 105){
    //M1_forward(speed_val);
    
      digitalWrite(M1_A, LOW);
      digitalWrite(M1_B, HIGH);
      analogWrite(M1_PWM, speed_val);
    
    
    analogWrite(LED_PWM, 254);
    delay(10);
  }
//    // if byte is equal to "106" or "j", go left
//  else if (x == 106){
//    //M1_reverse(speed_val);
//    M2_left(speed_val);
//    analogWrite(LED_PWM, 50);
//    delay(10);
//  }
//  // if byte is equal to "108" or "l", go right
//  else if (x == 108){
//    //M1_forward(speed_val);
//    M2_right(speed_val);
//    analogWrite(LED_PWM, 100);
//    delay(10);
//  }
//  // if byte is equal to "107" or "k", go reverse
//  else if (x == 107){
//    M1_reverse(speed_val);
//    //M2_left(speed_val);
//    analogWrite(LED_PWM, 200);
//    delay(10);
//  }
//  // otherwise, stop both motors
  else {
    M1_stop();
    M2_stop();
    analogWrite(LED_PWM, 0);
    delay (10);
  }

}
