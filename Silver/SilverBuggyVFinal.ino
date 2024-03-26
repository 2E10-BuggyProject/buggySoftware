
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
//ultra-sonic sensor trigger and echo pins
const int US_TRIG = 9;
const int US_ECHO = 13;

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
volatile int r_wheel_d = 0;
volatile int l_wheel_d = 0;

int PWM_speed = 100, global_speed = 100;
bool control_mode = 0;


double prev_dist = 0;
double distance_travelled = 0;



double kp = 5; // proportional constant
double ki = 0; // integral const
double kd = -3; // derivative const

double Skp = 1; // proportional constant
double Ski = 0; // integral const
double Skd = 0.5; // derivative const

unsigned long currentTime, previousTime;
double elapsedTime;
double error = 0, last_error = 0;
double input, output, set_point = 20;
double cum_error = 0, rate_error = 0;

double sprev_dist = 0;
double sdistance_travelled = 0;
double buggy_speed = 0;

unsigned long START = millis();
double dist1 = 0;
float reference_speed = 20.0;


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

  //start moving
  track_colour = digitalRead(LEYE); // surrounding track colour for initial state
  startMoving();
}

void loop() {

  cycles++;
  client = server.available();

  if(client.available()){
    instruction = client.read();
    Serial.println("Reading client.");
    
    if(instruction == 'w' || instruction == 's'){
      carry_out = instruction;
    }

    if(instruction == 'n'){
      String sinstruction = client.readStringUntil('n');
      reference_speed = sinstruction.toFloat();
      Serial.println(reference_speed);
    }
  }

  

  if(carry_out == 'w'){
    keepMoving = true;
  } else if(carry_out == 's'){
    keepMoving = false;
    stopMoving();
  }


  // if(keepMoving == true){


  //     computePID(distance);
  //     checkTurn();
  //     checkObstruction();
  //     distanceReport();
      
  //   }else{
  //     stopMoving();
  //   }

  

  if(keepMoving == true){

    if(control_mode == 0){
      computePID(distance);
      checkTurn();
      checkObstruction();
      distanceReport();
      
    }else if(control_mode == 1){
      speedComputePID(buggy_speed);
      checkTurn();
      checkObstruction();
      distanceReport();
    }
    
  }else {
    stopMoving();
  }

  if(cycles%30 == 0){
    Serial.print("PWM_speed: ");
    Serial.println(PWM_speed);
  

  }
  
  
  //calculate speed
  if((millis() - START) > 2000){
    buggy_speed = (distance_travelled - dist1)/2;

    START = millis();
    dist1 = distance_travelled;

     //Serial.println(reference_speed);
    String stringsss = "bs" + String(buggy_speed);
    auto mystr = stringsss.c_str();
    client.write(mystr);

  }



  //  Serial.print("buggy_speed: ");
  //  Serial.println(buggy_speed);

  //  Serial.print("distance_travelled: ");
  //  Serial.println(distance_travelled);

 

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

  //left turn condition
  if(leye_state  != track_colour && reye_state == track_colour){//left turn condition
    analogWrite(LSPEEDCTRL, 10);
    analogWrite(RSPEEDCTRL, (1.3*global_speed) + 10 );
  }else if(reye_state  != track_colour && leye_state == track_colour){ // right turn condition
    analogWrite(RSPEEDCTRL, 10);
    analogWrite(LSPEEDCTRL, (1.3*global_speed));
  }else{//continue straight
    analogWrite(LSPEEDCTRL, global_speed);
    analogWrite(RSPEEDCTRL, global_speed + 10 );
  }
}

void checkObstruction(){

    if(cycles%15 == 0){
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
        client.write("O");
        same_obstruction = true;
      }
      
    }else{
      digitalWrite(RMOTOR1, HIGH);
      digitalWrite(LMOTOR1, HIGH);
      same_obstruction = false;
      client.write("+");
    }
  }

  

  if(cycles%50==0){
    Serial.println(distance);
    auto tosend = ("us" + String(distance)).c_str();
    client.write(tosend);


  if(distance > 70){
      control_mode = 1;
      client.write("01");//client changed
  }else if(distance < 70){
      control_mode = 0;
      client.write("00");
  }

  }
}

void updateRwEnc(){
  r_wheel_d += 1;
}

void updateLwEnc(){
  l_wheel_d += 1;
}


void distanceReport(){
  


  double avg_revs = (0.5 * (r_wheel_d + l_wheel_d)) / 8;
  distance_travelled = avg_revs * 20.6;
  
  if(distance_travelled - prev_dist >= 36){
    prev_dist = distance_travelled;


    auto tosend = ("d" + String(distance_travelled)).c_str();
    client.write(tosend);

  }
  
}




void computePID(int US_read){

  if(cycles % 15 ==0){

  currentTime = millis();                //get current time
  elapsedTime = (double)(currentTime - previousTime);

  error = US_read - set_point;
  cum_error += error * elapsedTime;                // compute integral
  rate_error = (error - last_error)/elapsedTime;   // compute derivative

  double out = kp*error + ki*cum_error + kd*rate_error;                //PID output               

  last_error = error;                                //remember current error
  previousTime = currentTime;                        //remember current time

  //Serial.println(out);

  PWM_speed += out;

  if(PWM_speed < 70){
    PWM_speed = 70;
  }else if(PWM_speed > 130){
    PWM_speed = 130;
  }

  global_speed = PWM_speed;


  //Serial.print(PWM_speed);
  //buggy_speed
  }

  
  
}


void sendData(){
 //distance from sensor
 //mode of control
 //ref speed/
 //buggy speed
 //distance travelled

}

void speedComputePID(float SPEED_read){

 if((millis() - START) > 2000){

  currentTime = millis();                //get current time
  elapsedTime = (double)(currentTime - previousTime);


  error = SPEED_read - reference_speed;
  cum_error += error * elapsedTime;                // compute integral
  rate_error = (error - last_error)/elapsedTime;   // compute derivative

  double out = Skp*error + Ski*cum_error + Skd*rate_error;                //PID output               

  last_error = error;                                //remember current error
  previousTime = currentTime;                        //remember current time
  //sprev_dist = sdistance_travelled;

  //Serial.println(out);

  PWM_speed -= out;

  if(PWM_speed < 70){
    PWM_speed = 70;
  }else if(PWM_speed > 140){
    PWM_speed = 140;
  }
    

  } 
  global_speed = PWM_speed;
  
}




