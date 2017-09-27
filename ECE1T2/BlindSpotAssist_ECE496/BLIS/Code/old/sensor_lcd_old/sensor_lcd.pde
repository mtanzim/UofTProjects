#include <LiquidCrystal.h>


const int NUM_READINGS =3;//number of readings for distance array (needs to be at least 2 for the speed calculations to work;
const int NUM_SPEED_READINGS =5;//number of readings for speed array
const int TIME_DELAY=20;//delay time after the entire process
const int TRIGGER_DELAY=10; //how long to wait for echo (don't change this from 10)
const int MAX_DELTA_D=40; //maximum allowed distance delta b/w two intervals to steady speed calculations

int total=0;
int a_index=0;
int current_index=0;
int average_distance=0;
int current_distance=0;
int distance [NUM_READINGS];

//for speed calculations
unsigned long startTime=0 ;                    // start time for stop watch
unsigned long elapsedTime=0;                  // elapsed time for stop watch
int old_distance=0;
int delta_d=0;
float inst_speed=0.0;
float average_speed=0.0;
float total_speed=0.0;
float speeds [NUM_SPEED_READINGS];
int s_index=0;

float time_left=0.0;    //time left until distance is zero




//hardware variables setup
int pin_echo=2;  //srf05 echo pin (dig 2)
int pin_trigger=3 ;  //srf05 trigger pin (dig 3)
int led_trigger=13; // (digital 13)
unsigned long pulseTime=0;  //stores pulse in microseconds

//lcd definition
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

//setup
void setup () {
   pinMode (pin_trigger, OUTPUT);
   pinMode (pin_echo, INPUT);
   
   //LED setup
   pinMode (led_trigger, OUTPUT); //sets dig. pin as output
   
   lcd.begin(16, 2); //sets up the lcd
   
   for (int cur_reading = 0; cur_reading < NUM_READINGS ; cur_reading ++) {
    distance [cur_reading] = 0;
  }
  
  //initialize speed array
   for (int cur_reading = 0; cur_reading < NUM_SPEED_READINGS ; cur_reading ++) {
    speeds [cur_reading] = 0.0;
  }
  Serial.begin(9600);
}



//external function to avoid rounding errors:
//found from:
//http://www.arduino.cc/playground/Code/PrintFloats
// printFloat prints out the float 'value' rounded to 'places' places after the decimal point
void printFloat(float value, int places) {
  // this is used to cast digits 
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;

    // make sure we round properly. this could use pow from <math.h>, but doesn't seem worth the import
  // if this rounding step isn't here, the value  54.321 prints as 54.3209

  // calculate rounding term d:   0.5/pow(10,places)  
  float d = 0.5;
  if (value < 0)
    d *= -1.0;
  // divide by ten for each decimal place
  for (i = 0; i < places; i++)
    d/= 10.0;    
  // this small addition, combined with truncation will round our values properly 
  tempfloat +=  d;

  // first get value tens to be the large power of ten less than value
  // tenscount isn't necessary but it would be useful if you wanted to know after this how many chars the number will take

  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }


  // write out the negative if needed
  if (value < 0)
    Serial.print('-');

  if (tenscount == 0)
    Serial.print(0, DEC);

  for (i=0; i< tenscount; i++) {
    digit = (int) (tempfloat/tens);
    Serial.print(digit, DEC);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }

  // if no places after decimal, stop now and return
  if (places <= 0)
    return;

  // otherwise, write the point and continue on
  Serial.print('.');  

  // now write out each decimal place by shifting digits one by one into the ones place and writing the truncated value
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0; 
    digit = (int) tempfloat;
    Serial.print(digit,DEC);  
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit; 
  }
}





void loop () {
 
  digitalWrite (pin_trigger, HIGH); //drive trigger pin (10ms)
  delayMicroseconds(TRIGGER_DELAY);//10ms wait
  digitalWrite (pin_trigger, LOW); //stop sending pulses
  
  
  
  pulseTime=pulseIn(pin_echo, HIGH); //get the returned pulse from srf05
  
  elapsedTime = micros() - startTime;
  startTime = micros();
  //float_t=float(elapsedTime)/float(1000000);


  
  current_distance = pulseTime/58; //conversion ratio according to spec sheet
  total=total-distance[a_index];
  distance[a_index]=current_distance;  //store current value of distance in distance array
  total=total+distance[a_index];
  
  
    
  if (a_index==0) {
    old_distance=distance[NUM_READINGS-1];
  }  
  else {
    old_distance=distance[a_index-1];
  }
    
  a_index += 1;   //INCREMENT COUNTER
  
  if (a_index >= NUM_READINGS ) {
    a_index=0;
  }
  
  
  average_distance=total/NUM_READINGS;
  
  //do speed calculations and averaging
  delta_d = current_distance - old_distance; //difference b/w last two distances
  if (abs(delta_d) > MAX_DELTA_D ) {
      delta_d=0;
     
  }
  else {
    inst_speed= float(delta_d)/((float(elapsedTime))/float(1000000.00));
    total_speed=total_speed-speeds[s_index]; //get rid of last value in array
    speeds [s_index] = inst_speed;
    total_speed=total_speed+speeds[s_index];
    
    s_index +=1;
    
    if (s_index >= NUM_SPEED_READINGS ) {
      s_index=0;
    }
    
    average_speed=total_speed/float(NUM_SPEED_READINGS);
    time_left = float(current_distance)/average_speed;
    
  }
//  Serial.print("a_index: ");
//  Serial.println(a_index, DEC);
  //Serial.print("d: ");
  Serial.print(current_distance, DEC);
//  Serial.print("old_d: ");
//  Serial.println(old_distance, DEC);
  //Serial.print("delta_t: ");
  //Serial.println(elapsedTime);
  //Serial.println();
  //Serial.print("delta_d: ");
  //Serial.println(delta_d, DEC);
  //Serial.print("inst_speed: ");
  //Serial.println();
  //if (average_speed != 0) { 
    //Serial.println(current_distance, DEC); 
    //Serial.print("    "); 
   //printFloat(average_speed, 7);
   
   //printFloat(time_left, 5); 
  //}
  
  //Serial.print("average_speed: ");
  printFloat(average_speed, 7);
  //Serial.print("-----");
  Serial.println();
  
 lcd.clear(); 
    // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // print distance from obstacle
  
  lcd.print(current_distance);
  //lcd.print("");
  
  lcd.setCursor(0, 1);
  lcd.print(average_speed);
  
  delay (TIME_DELAY);

  
}
  
  
