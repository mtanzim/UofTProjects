int M1_PWM = 11; // PWM H-bridge enable pin for speed control
int M1_A = 12; // H-bridge leg 1 Motor 1
int M1_B = 10; // H-bridge leg 2 Motor 1




int M2_PWM = 3; // PWM H-bridge enable pin for speed control
int M2_A = 2; // H-bridge leg 1 Motor 2
int M2_B = 4; // H-bridge leg 2 Motor 2




int LED_PWM=6;

int speed_val=200;

int incomingByte=0;



void setup(){

  Serial.begin(9600);
  
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_A, OUTPUT);
  pinMode(M1_B, OUTPUT);
  
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_A, OUTPUT);
  pinMode(M2_B, OUTPUT);
  
  

  
  
  
  
  pinMode(LED_PWM, OUTPUT);







}

////////////////////////////////////

void loop(){


  if (Serial.available() > 0) { // check for serial data
    incomingByte = Serial.read(); // read the incoming byte
    
    Serial.print("I received: "); // say what you got
    Serial.println(incomingByte); // incomingByte takes value from the serial read
    
    // delay 10 milliseconds to allow serial update time
    delay(10);
  
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



