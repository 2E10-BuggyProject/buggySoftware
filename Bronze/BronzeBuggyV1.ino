
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


void setup() {
  Serial.begin(9600);

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


  leye_state = digitalRead(LEYE);
  reye_state = digitalRead(REYE);

  //right turn condition
  if(leye_state  != track_colour && reye_state == track_colour){
    //Serial.println("Stopping left");
    //digitalWrite(LMOTOR1, LOW);
    analogWrite(LSPEEDCTRL, 20);
    analogWrite(RSPEEDCTRL, 250);
  }else{
    //Serial.println("Initializing left");
    //digitalWrite(LMOTOR1, HIGH);
    analogWrite(LSPEEDCTRL, 170);
    analogWrite(RSPEEDCTRL, 160);
  }
  //right turn condition
  if(reye_state  != track_colour && leye_state == track_colour){
    //Serial.println("Stopping right");
    //digitalWrite(RMOTOR1, LOW);
    analogWrite(RSPEEDCTRL, 10);
    analogWrite(LSPEEDCTRL, 250);
  }else{
    //Serial.println("Initializing right");
    //digitalWrite(RMOTOR1, HIGH);
    analogWrite(LSPEEDCTRL, 170);
    analogWrite(RSPEEDCTRL, 160);
  }

  //check for obstacles every x iterations
  if(cycles%10 == 0){
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
    }else{
      digitalWrite(RMOTOR1, HIGH);
      digitalWrite(LMOTOR1, HIGH);
    }

  }
  cycles++;
  
}

void startMoving(){
  
  digitalWrite(RMOTOR1, HIGH);
  digitalWrite(RMOTOR2, LOW);
  digitalWrite(LMOTOR1, HIGH);
  digitalWrite(LMOTOR2, LOW);

  analogWrite(LSPEEDCTRL, 170);
  analogWrite(RSPEEDCTRL, 160);
}

void stopMoving(){
  digitalWrite(RMOTOR1, LOW);
  digitalWrite(LMOTOR1, LOW);

}

void checkObstruction(){

    digitalWrite( US_TRIG, LOW ); 
    delayMicroseconds(2); 
    
    digitalWrite( US_TRIG, HIGH ); 
    delayMicroseconds( 10 ); 
    digitalWrite( US_TRIG, LOW ); 
    duration = pulseIn( US_ECHO, HIGH ); 
    Serial.println(duration);
    Serial.println(distance);
    distance = (duration/2.0) / 29.1;
}
