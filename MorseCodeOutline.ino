
const int LED_PIN = 13;
const int time_unit = 100;
String message = "FUck arduino";

String morsetable[] = {
    // Letters A-Z (indices 0-25)
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....",
    "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.",
    "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-",
    "-.--", "--..", 
    // Digits start here (indices 26-35)
    ".----", "..--", "...--", "....-", "....",
    "-...", "--...", "---..", "----.", "-----"
};


/* Convert a character to an index into the morse code table.
 *  Rules:
 *    If it's an uppercase letter (A-Z) then return 0-25
 *    If it's a digit (0-9) then return 26-35
 *    Anything else return -1
 *  Assumption:
 *    The character has already been converted to uppercase if it's a letter.
 */
int lookup(char c){

  // If the character is not a valid letter or digit then return -1
  if( !isAlphaNumeric(c) ){
    return -1;
  }


  // Now we do a bit of ASCII trickery to convert letters & digits
  // into positions in our lookup table. See https://www.arduino.cc/reference/en/language/variables/data-types/char/
  // Letters are easier
  if( isAlpha(c) ){
    return c - 'A';
  }

  if( isDigit(c) ){
    // add on 25 because the digits come after the letters in the table, so we skip that far
    return (c - '0') + 25; 
  }

}

void flash(char c){

  if(c == '.'){
    Serial.print("dot");
    digitalWrite(LED_PIN, HIGH);
    delay(time_unit);
    digitalWrite(LED_PIN, LOW);
  }

  if(c == '-'){
    Serial.print("dash");
    digitalWrite(LED_PIN, HIGH);
    delay(3*time_unit);
    digitalWrite(LED_PIN, LOW);
  }

  delay(5*time_unit);
}


void setup() {
Serial.begin(9600);
pinMode(LED_PIN, OUTPUT);
Serial.println("initialize");

}

void loop() {

message.toUpperCase();

message.trim();

for(auto c : message){
  String y = morsetable[lookup(c)];
  //Serial.print(y);
  //Serial.print("         ");

  for(auto pm : y){
    Serial.print(pm);
    flash(pm);
  }

}  




}
