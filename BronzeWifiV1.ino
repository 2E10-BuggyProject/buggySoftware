#include <WiFiS3.h>
//network settings
char ssid[] = "Y1_AP";
char pass[] = "bababooey";
WiFiServer server(5200);

//IR sensors
const int LEYE = 3;
const int REYE = 4;
//Motor logic controlls
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

String instruction;
bool keepMoving = true;



void setup() {
  Serial.begin(9600);
  //wifi setup
  WiFi.beginAP(ssid, pass);
  Serial.print("Access Point IP Address: ");
  Serial.println(WiFi.localIP());
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

  //start moving
  track_colour = digitalRead(LEYE); // surrounding track colour for initial state
  startMoving();
}

void loop() {

  WiFiClient client = server.available();
  
  if(client.connected()){
    instruction = client.readStringUntil('\n');
    Serial.println(instruction);

    if(instruction == "go"){
      keepMoving = true;
      startMoving();
    }else if(String(instruction) == "stop"){
      keepMoving = false;
    }

    if(keepMoving){
      checkTurn();
      //check for obstacles every x iterations
      checkObstruction();
      cycles++;

    }else if(!keepMoving){
      stopMoving();

    }
  }
}

void startMoving(){
  
  digitalWrite(RMOTOR1, HIGH);
  digitalWrite(RMOTOR2, LOW);
  digitalWrite(LMOTOR1, HIGH);
  digitalWrite(LMOTOR2, LOW);

  analogWrite(LSPEEDCTRL, 90);
  analogWrite(RSPEEDCTRL, 91);
}

void stopMoving(){
  digitalWrite(RMOTOR1, LOW);
  digitalWrite(LMOTOR1, LOW);

}

void checkTurn(){
  
  leye_state = digitalRead(LEYE);
  reye_state = digitalRead(REYE);

  //left turn condition
  if(leye_state  != track_colour && reye_state == track_colour){
    Serial.println("Stopping left");
    //digitalWrite(LMOTOR1, LOW);
    analogWrite(LSPEEDCTRL, 10);
    analogWrite(RSPEEDCTRL, 135);
  }else if(reye_state  != track_colour && leye_state == track_colour){ // right turn condition
    Serial.println("Stopping right");
    //digitalWrite(RMOTOR1, LOW);
    analogWrite(RSPEEDCTRL, 10);
    analogWrite(LSPEEDCTRL, 132);
  }else{
    Serial.println("Initializing");
    //digitalWrite(LMOTOR1, HIGH);
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
    //Serial.println(duration);
    Serial.println(distance);
    distance = (duration/2.0) / 29.1;

    if(distance<10){
      stopMoving();
      //client.println("Obstruction!");
    }else{
      digitalWrite(RMOTOR1, HIGH);
      digitalWrite(LMOTOR1, HIGH);
      //client.println("Moving off");
    }

  }
}
