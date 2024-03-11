#include <WiFiS3.h>
//network settings
char ssid[] = "Shanes iphone";
char pass[] = "bababooey";
WiFiServer server(5200);
WiFiClient client;

//IR sensors
const int LEYE = 3;
const int REYE = 4;
//Motor logic controlls (forward/back/on/off)
const int RMOTOR1 = 10;
const int RMOTOR2 = 11;
const int LMOTOR1 = 12;
const int LMOTOR2 = 13;
//motor PWM speed controlls
const int LSPEEDCTRL = 6;
const int RSPEEDCTRL = 5;
//ultra-sonic sensor trigger and echo pins
const int US_TRIG = 9;
const int US_ECHO = 8;

bool leye_state = 0;//high when sees black, updates in loop
bool reye_state = 0;
bool track_colour; // initializes to the surrounding IR read

int distance; //distance to obstruction
long duration; //used to calculate distance for stop condition

int cycles = 0; //how often to check for obstructions
//instruction params
char instruction, carry_out;
bool keepMoving = false, same_obstruction = false;
//wheel encoders
const int RW_ENC = 2;
const int LW_ENC = 7;
volatile int r_wheel_d = 0;
volatile int l_wheel_d = 0;


void setup() {
  Serial.begin(9600);
  //wifi setup
  WiFi.begin(ssid, pass);
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
    
    if(instruction == 'w' || 's'){
      carry_out = instruction;
    }

    Serial.println(instruction);
  }

  if(carry_out == 'w'){
    keepMoving = true;
  } else if(carry_out == 's'){
    keepMoving == false;
    stopMoving();
  }

  if(keepMoving){
    checkTurn();
    checkObstruction();
    distanceReport();
  }else{
    stopMoving();
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

  //left turn condition
  if(leye_state  != track_colour && reye_state == track_colour){//left turn condition
    analogWrite(LSPEEDCTRL, 10);
    analogWrite(RSPEEDCTRL, 135);
  }else if(reye_state  != track_colour && leye_state == track_colour){ // right turn condition
    analogWrite(RSPEEDCTRL, 10);
    analogWrite(LSPEEDCTRL, 132);
  }else{//continue straight
    analogWrite(LSPEEDCTRL, 90);
    analogWrite(RSPEEDCTRL, 91);
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
        client.write("Obstruction!");
        same_obstruction = true;
      }
      
    }else{
      digitalWrite(RMOTOR1, HIGH);
      digitalWrite(LMOTOR1, HIGH);
      same_obstruction = false;
    }
  }
}

void updateRwEnc(){
  r_wheel_d += 1;
}

void updateLwEnc(){
  l_wheel_d += 1;
}

int prev_dist = 0;
void distanceReport(){
  float avg_revs = 0.5 * (r_wheel_d + l_wheel_d);
  int distance = avg_revs * 18;
  if(distance - prev_dist > 60){
    prev_dist = distance;
    //Serial.println(l_wheel_d);
    //Serial.println(r_wheel_d);

    client.write("+");
  }
  
  //float distance_travelled = (18) * avg_revs;

  /*if(distance_travelled - prev_dist > 18){
    prev_dist = distance_travelled;
    //Serial.println(distance_travelled);
    client.write('+');*/
  
}