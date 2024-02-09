

const int LEYE = 5;
const int REYE = 6;

const int RMOTOR1 = 10;
const int RMOTOR2 = 11;

bool leye_val;
bool reye_val;


void setup() {
  Serial.begin(9600);

  pinMode( LEYE, INPUT );
  pinMode( REYE, INPUT );

  pinMode( RMOTOR1, OUTPUT );
  pinMode( RMOTOR2, OUTPUT );

  

  leye_val = 0;
  reye_val = 0;
}

void loop() {
  bool new_leye;
  bool new_reye;


  new_leye = digitalRead( LEYE );
  if(new_leye  != leye_val ){
    leye_val = new_leye; 
    Serial.println("Left eye: " + String(leye_val));
  }

  new_reye = digitalRead( REYE );
  if(new_reye  != reye_val ){
    reye_val = new_reye; 
    Serial.println("Right eye: " + String(reye_val));
    digitalWrite(RMOTOR1, !new_reye);
    digitalWrite(RMOTOR2, LOW);
    
  }
  
  delay(100);
}
