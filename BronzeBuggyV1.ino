
// IR sensor pins
const int LEYE = 3;
const int REYE = 4;
//Motor controll pins
const int RMOTOR1 = 10;
const int RMOTOR2 = 11;
const int LMOTOR1 = 12;
const int LMOTOR2 = 13;
//PWM speed controlls for turning, etc
const int LSPEEDCTRL = 6;
const int RSPEEDCTRL = 5;


bool leye_state = 0;//high when sees black, updates in loop
bool reye_state = 0;
bool track_colour; // initializes to the surrounding IR read


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

  //start moving
  startMoving();

}

void loop() {

  leye_state = digitalRead(LEYE);
  if(leye_state  != track_colour ){
    Serial.println("Stopping left");
    //digitalWrite(LMOTOR1, LOW);
    analogWrite(LSPEEDCTRL, 50);
    analogWrite(RSPEEDCTRL, 200);
  }else{
    Serial.println("Initializing left");
    //digitalWrite(LMOTOR1, HIGH);
    analogWrite(LSPEEDCTRL, 160);
    analogWrite(RSPEEDCTRL, 160);
  }

  reye_state = digitalRead(REYE);
  if(reye_state  != track_colour ){
    Serial.println("Stopping right");
    //digitalWrite(RMOTOR1, LOW);
    analogWrite(RSPEEDCTRL, 50);
    analogWrite(LSPEEDCTRL, 200);
  }else{
    Serial.println("Initializing right");
    //digitalWrite(RMOTOR1, HIGH);
    analogWrite(LSPEEDCTRL, 160);
    analogWrite(RSPEEDCTRL, 160);
  }
  
}


void startMoving(){
  track_colour = digitalRead(LEYE); // surrounding track colour for initial state
  digitalWrite(RMOTOR1, HIGH);
  digitalWrite(RMOTOR2, LOW);
  digitalWrite(LMOTOR1, HIGH);
  digitalWrite(LMOTOR2, LOW);

  analogWrite(LSPEEDCTRL, 120);
  analogWrite(RSPEEDCTRL, 120);
}


