/*************************************************************************************
 2E10 Gold Submission Group Y1 
 Ruairi Mullally, Shane McDermott, Noah Savage, Labiba Mansur

 Gold version has follow mode ommitted, as april tags would be detected and cause the 
 buggy to stop moving.
*************************************************************************************/

//computer vision camera lib
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

HUSKYLENS huskylens;
int tag_id = 0; //april tag id
bool turning_left = false, turning_right = false;

#include <WiFiS3.h>
//network settings
char ssid[] = "Y1_AP";
char pass[] = "bababooey";
WiFiServer server(5200);
WiFiClient client;

//IR sensors
const int LEYE = 7;
const int REYE = 4;
//Motor logic controlls (forward/back/on/off)
const int RMOTOR1 = 10;
const int RMOTOR2 = 11;
const int LMOTOR1 = 12;
const int LMOTOR2 = 8;
//motor PWM speed controlls
const int LSPEEDCTRL = 6;
const int RSPEEDCTRL = 5;
//ultra-sonic sensor trigger and echo pins for obstacle detection
const int US_TRIG = 9;
const int US_ECHO = 13;
//IR sensor states
bool leye_state = 0;//high when sees black, updates in loop
bool reye_state = 0;
bool track_colour; // initializes to the surrounding IR read

double distance = 0; //distance to obstruction
long duration; //used to calculate distance for stop condition
int cycles = 0; //how often to check for obstructions

//instruction params
char instruction, carry_out;
bool keepMoving = false, same_obstruction = false;
//wheel encoders
const int RW_ENC = 2;
const int LW_ENC = 3;
volatile int r_wheel_d = 0;//wheel encoder falling signal counter
volatile int l_wheel_d = 0;
//control wheel speed with PID
int PWM_speed = 100, global_speed = 100;
bool control_mode = 0;
//log travel distance
double prev_dist = 0;
double distance_travelled = 0;
//follow PID constants (will differ based on buggy weight, etc.)
double kp = 5; // proportional constant
double ki = 0; // integral const
double kd = -3; // derivative const
//speed PID constants
double Skp = 1.0; // proportional constant
double Ski = 0; // integral const
double Skd = 0.5; // derivative const
//PID error calc
unsigned long currentTime, previousTime;
double elapsedTime;
double error = 0, last_error = 0;
double input, output, set_point = 20;
double cum_error = 0, rate_error = 0;
//buggy speed calc params
double buggy_speed = 0;
unsigned long START = millis();
double dist1 = 0;//used to calculate buggy speed
float reference_speed = 25.0;//set remotely from GUI


void setup() {
  Serial.begin(9600);
  //wifi setup
  WiFi.beginAP(ssid, pass);
  Serial.print("Network IP Address: ");
  Serial.print(WiFi.localIP());
  server.begin();

  //configure pins
  //IR sensors
  pinMode( LEYE, INPUT );
  pinMode( REYE, INPUT );
  //motor on/off states
  pinMode( RMOTOR1, OUTPUT );
  pinMode( RMOTOR2, OUTPUT );
  pinMode( LMOTOR1, OUTPUT );
  pinMode( LMOTOR2, OUTPUT );
  //US sensor
  pinMode(US_TRIG, OUTPUT);
  pinMode(US_ECHO, INPUT);
  //wheel encoders
  pinMode(LW_ENC,INPUT_PULLUP);
  pinMode(RW_ENC,INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt(LW_ENC), updateLwEnc, RISING);
  attachInterrupt( digitalPinToInterrupt(RW_ENC), updateRwEnc, RISING);

  initialiseHusky();//start husky lens
  //start moving
  track_colour = digitalRead(LEYE); // surrounding track colour for initial state
  startMoving();
}

void loop() {
  cycles++;
  client = server.available();

  if(client.available()){//if client has message to read
    instruction = client.read();
    Serial.println("Reading client.");
    
    if(instruction == 'w' || instruction == 's'){//movement instructions
      carry_out = instruction;
    }

    if(instruction == 'n'){//reference speed set
      String sinstruction = client.readStringUntil('n');
      reference_speed = sinstruction.toFloat();
      Serial.print("\nNew reference speed set: ");
      Serial.println(reference_speed);
    }
  }

  if(carry_out == 'w'){//stop or go instruction from GUI
    keepMoving = true;
  } else if(carry_out == 's'){
    keepMoving = false;
    stopMoving();
  }

  if(keepMoving == true){

    speedComputePID(buggy_speed);
    checkTurn();
    checkObstruction();
    detectTags();
    distanceReport();
    
  }else {
    stopMoving();
  }
  
  //calculate speed
  if((millis() - START) >= 2000){
    buggy_speed = (distance_travelled - dist1)/2;

    START = millis();
    dist1 = distance_travelled;

    //send buggy speed to client for GUI
    auto tosend = ("bs" + String(buggy_speed)).c_str();
    client.write(tosend);

  }
}

void startMoving(){
  //set motors to forward
  digitalWrite(RMOTOR1, HIGH);
  digitalWrite(RMOTOR2, LOW);
  digitalWrite(LMOTOR1, HIGH);
  digitalWrite(LMOTOR2, LOW);
  //set motor speed
  analogWrite(LSPEEDCTRL, 90);
  analogWrite(RSPEEDCTRL, 91);
}

void stopMoving(){
  //turn off motors
  digitalWrite(RMOTOR1, LOW);
  digitalWrite(LMOTOR1, LOW);
}

void checkTurn(){
  
  leye_state = digitalRead(LEYE);
  reye_state = digitalRead(REYE);


  if(leye_state  != track_colour && reye_state != track_colour && turning_left == true){//left turn condition for intersection
    analogWrite(LSPEEDCTRL, 10);
    analogWrite(RSPEEDCTRL, (1.3*global_speed) + 10 ); // right motor weaker than left motor
    Serial.println("Turning left from tag");
  } else if(leye_state  != track_colour && reye_state != track_colour && turning_right == true){//right turn condition for intersection
    analogWrite(RSPEEDCTRL, 10);
    analogWrite(LSPEEDCTRL, (1.3*global_speed));
    Serial.println("Turning right from tag");
  }else if(leye_state  != track_colour && reye_state == track_colour){//left turn condition
    analogWrite(LSPEEDCTRL, 10);
    analogWrite(RSPEEDCTRL, (1.3*global_speed) + 10 ); // right motor weaker than left motor
  }else if(reye_state  != track_colour && leye_state == track_colour){ // right turn condition
    analogWrite(RSPEEDCTRL, 10);
    analogWrite(LSPEEDCTRL, (1.3*global_speed));
  }else{//continue straight
    analogWrite(LSPEEDCTRL, global_speed);
    analogWrite(RSPEEDCTRL, global_speed + 10 );
  }
}

void checkObstruction(){

    if(cycles%15 == 0){//as process is quite time intensive, only do every 15 cycles
    //set trigger low
    digitalWrite( US_TRIG, LOW ); 
    delayMicroseconds(2); 
    //send pulse
    digitalWrite( US_TRIG, HIGH ); 
    delayMicroseconds( 10 ); 
    digitalWrite( US_TRIG, LOW ); 
    //find time for pulse to return
    duration = pulseIn( US_ECHO, HIGH ); 
    distance = (duration/2.0) / 29.1;

    if(distance<10){
  
      stopMoving();
      if(!same_obstruction){
        client.write("O"); //inform client
        same_obstruction = true;
      }
      
    }else{
      digitalWrite(RMOTOR1, HIGH);
      digitalWrite(LMOTOR1, HIGH);
      same_obstruction = false;
      client.write("+");//inform moving off
    }
  }

  

  if(cycles%50==0){ // every 50 cycles send distance to object ahead
    Serial.println(distance);
    auto tosend = ("us" + String(distance)).c_str();
    client.write(tosend);


    if(distance > 40){//if no objects in range switch to reference speed
        control_mode = 1;
        client.write("01");//inform client of mode change
    }else if(distance <= 40){//otherwise switch to follow mode
        control_mode = 0;
        client.write("00");
    }
  }
}

void updateRwEnc(){//add count to right wheel encoder
  r_wheel_d += 1;
}

void updateLwEnc(){//add count to left wheel encoder
  l_wheel_d += 1;
}


void distanceReport(){ //calculates distance travelled and informs client
  
  double avg_revs = (0.5 * (r_wheel_d + l_wheel_d)) / 8;//8 magnets ineach wheel encoder
  distance_travelled = avg_revs * 20.6;//wheel circumference
  
  if(distance_travelled - prev_dist >= 36){
    prev_dist = distance_travelled;

    auto tosend = ("d" + String(distance_travelled)).c_str();
    client.write(tosend);

  }
}


void computePID(int US_read){//distance follow PID

  if(cycles % 15 ==0){

    currentTime = millis();                //get current time
    elapsedTime = (double)(currentTime - previousTime);

    error = US_read - set_point;
    cum_error += error * elapsedTime;                // compute integral
    rate_error = (error - last_error)/elapsedTime;   // compute derivative

    double out = kp*error + ki*cum_error + kd*rate_error;                //PID output               

    last_error = error;                                //remember current error
    previousTime = currentTime;                        //remember current time

    PWM_speed += out;

    if(PWM_speed < 70){//speed bounds for stability
      PWM_speed = 70;
    }else if(PWM_speed > 130){
      PWM_speed = 130;
    }

    global_speed = PWM_speed;
  }
}


void speedComputePID(float SPEED_read){//ref speed PID

  if((millis() - START) > 2000){

    currentTime = millis();                //get current time
    elapsedTime = (double)(currentTime - previousTime);

    error = SPEED_read - reference_speed;
    cum_error += error * elapsedTime;                // compute integral
    rate_error = (error - last_error)/elapsedTime;   // compute derivative

    double out = Skp*error + Ski*cum_error + Skd*rate_error;                //PID output               

    last_error = error;                                //remember current error
    previousTime = currentTime;                        //remember current time

    PWM_speed -= out;

    if(PWM_speed < 70){//speed bounds for stability
      PWM_speed = 70;
    }else if(PWM_speed > 160){
      PWM_speed = 160;
    }
    global_speed = PWM_speed; 
  } 
  
}

void initialiseHusky(){
  Wire.begin();

  while( !huskylens.begin(Wire) ){
    Serial.println( F("Huskylens begin failed!") );
    Serial.println( F("Check Huskylens protocol is set to I2C (General > Settings > Protocol Type > I2C") );
    Serial.println( F("And confirm the physical connection."));
    delay(1000); // Wait a second before trying to initialise again.
  }
}


void detectTags(){//detects april tag id and updates instructions

  if (huskylens.request() && huskylens.isLearned() && huskylens.available())
  {
    HUSKYLENSResult result = huskylens.read();
    
    tag_id = (result.ID);
  }

  switch(tag_id){
    
    case 1://speed up
    //Serial.println("case 1");
    reference_speed = 20;
    break;

    case 2://slow down
    //Serial.println("case 2");
    reference_speed = 10;
    break;

    case 3: // turn left at next intersection
    //Serial.println("case 3");
    turning_right = false;
    turning_left = true;
    break;

    case 4: // turn right at next intersection
    //Serial.println("case 4");
    turning_right = true;
    turning_left = false;
    break;

    default:

    //Serial.println("No tag detected");
    break;
  }
  tag_id = 0;
}
