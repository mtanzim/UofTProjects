
#include <Wire.h>

const int SLAVE_ADDRESS=4;

int M1_PWM = 11; // PWM H-bridge enable pin for speed control
int M1_A = 12; // H-bridge leg 1 Motor 1
int M1_B = 10; // H-bridge leg 2 Motor 1


int M2_PWM = 3; // PWM H-bridge enable pin for speed control
int M2_A = 2; // H-bridge leg 1 Motor 2
int M2_B = 4; // H-bridge leg 2 Motor 2

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
  
  
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_A, OUTPUT);
  pinMode(M1_B, OUTPUT);
  
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_A, OUTPUT);
  pinMode(M2_B, OUTPUT);

}

////////////////////////////////////

void loop(){
  delay (100);

}



/////////// motor functions ////////////////




void M1_stop(){
  
  digitalWrite(M1_PWM, LOW);
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_A, LOW);
  
}

void M2_stop(){
  
  digitalWrite(M2_PWM, LOW);
  digitalWrite(M2_B, LOW);
  digitalWrite(M2_A, LOW);
  
}



void M1_forward(int x){
  //analogWrite(LED_PWM, 255);
  
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_A, LOW);
  
  
  digitalWrite(M1_A, LOW);
  digitalWrite(M1_B, HIGH);
  analogWrite(M1_PWM, x);
  
}


void M1_reverse(int z){
  
  digitalWrite(M1_B, LOW);
  digitalWrite(M1_A, LOW);
  
  
  digitalWrite(M1_A, HIGH);
  digitalWrite(M1_B, LOW);
  analogWrite(M1_PWM, z);
  
}




void M2_right(int y){
  
  digitalWrite(M2_A, LOW);
  digitalWrite(M2_B, LOW); 
  
  digitalWrite(M2_A, HIGH);
  digitalWrite(M2_B, LOW);
  analogWrite(M2_PWM, y);
}

void M2_left(int y){
  
  digitalWrite(M2_A, LOW);
  digitalWrite(M2_B, LOW);
  
  digitalWrite(M2_A, LOW);
  digitalWrite(M2_B, HIGH);
  analogWrite(M2_PWM, y);
}


void M1_change_speed(int z){
  
  analogWrite(M1_PWM, z);
  
}




void receiveEvent(int howMany)
{
//  while(1 < Wire.available()) // loop through all but the last
//  {
//    char c = Wire.receive(); // receive byte as a character
//    //Serial.print(c);         // print the character
//  }
  int incomingByte = Wire.receive();    // receive byte as an integer
  //Serial.println(x);         // print the integer
  
  
  
  //digitalWrite(LED, HIGH); //quick debug
  //analogWrite(LED_PWM, 255);
    // if byte is equal to "105" or "i", go forward
    // if byte is equal to "105" or "i", go forward
    if (incomingByte == 105){
      M1_forward(speed_val);
      delay(100);
      
    }
    else if (incomingByte == 107){

      M1_reverse(speed_val);
      delay(100); 
    }
    // check incoming byte for direction
    //go left, j
    else if (incomingByte == 106){

      M2_left(254);
      delay(100); 
    }
    
    //go righ, l
    else if (incomingByte == 108){

      M2_right(254);
      delay(100); 
    }
    
    
    
    // if byte is equal to "46" or "," - raise speed
    else if (incomingByte == 97){
      speed_val = speed_val + 10;
      
      if (speed_val > 255) {
        speed_val=255;
      }
      M1_change_speed (speed_val);
      delay(100);
      //Serial.println(speed_val);
    }
    else if (incomingByte == 115){
      speed_val = speed_val - 10;
      
      if (speed_val < 200) {
        speed_val=200;
      }
      M1_change_speed (speed_val);
      delay(100);
      //Serial.println(speed_val);
    }
    
    
    
    else {
      M1_stop();
      M2_stop();
      delay(100);
    }
  }

}
