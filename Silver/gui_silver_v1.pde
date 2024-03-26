import controlP5.*;
import processing.net.*;
//import meter.*;

int distance =   0;

ControlP5 p5;
Button GOButton;
Button STOPButton;
Knob ModeKnob; 
RadioButton CruiseModes; 
Textfield textbox;
Textarea textoutput;
Println console;
Client myclient;
PFont font, pFont;
String data, obs_seen = "clear";
String input;  // for the switch, just to check 



void setup() {
  size(1600, 1000);
  background(0);
  pFont = createFont("Arial",16,true);
  
  textFont(pFont,20);                  // STEP 3 Specify font to be used
  fill(255);                         // STEP 4 Specify font color
  text("Distance: ",100,100);
  
  p5 = new ControlP5(this);
 
  GOButton = p5.addButton("buggy ON")
               .setValue(200)
               .setPosition(400,200)
               .setSize(400,80)
               .setColorBackground(color(0, 255, 0))
               .setFont(createFont("calibri", 40));
 
  STOPButton = p5.addButton("buggy OFF")
                 .setValue(0)
                 .setPosition(400,400)
                 .setSize(400,80)
                 .setColorBackground(color(255, 0, 0))
                 .setFont(createFont("calibri", 40));
                 

 
  textbox = p5.addTextfield("text")
               .setPosition(400, 600)
               .setSize(400, 80)
               .setAutoClear(true)
               .setFont(createFont("calibri", 40))
               .setColorBackground(color(255, 255, 255))
               .setColor(color(0));
               
  textoutput = p5.addTextarea("CONSOLE")
               .setPosition(900, 600)
               .setSize(400, 80)
               .setFont(createFont("calibri", 20))
               .setColorBackground(color(255, 255, 255))
               .setColor(color(0));  
               
  console = p5.addConsole(textoutput);     
  
  CruiseModes = p5.addRadioButton("CruiseModes")
                  .setSize(25,25)
                  .setPosition(900, 400)
                  .addItem("LOW", 0.3)
                  .addItem("MEDIUM", 0.6)
                  .addItem("FAST", 1); 
            
  myclient = new Client(this, "172.20.10.3", 5200);
  myclient.write("connecting");
  // code to connect to arduino
  font = createFont("calibri", 25);
}

void draw() {
  background(0);
  
  textFont(pFont,40);                 
  fill(255);                         
  text("Distance: " + distance + "cm",900,250);
  
  textFont(pFont,40);                 
  fill(255);                         
  text("Obstruction: "+obs_seen,900,550);
  
  
  textFont(pFont, 40);
  text("Group Y1", 200, 80);
  // fror aduino
 //dataC = myclient.readChar(); 
 data = myclient.readString();
 if(data != null){
   println(data);
   
   
 }


  
  try{
    if (data.equals("+")){ 
      distance += 28;
      obs_seen = "Clear.";
    }
    
    if(data.equals("Obstruction!")){
      obs_seen = "Obstruction!";
      
    }
    
  }catch(Exception e){
  }
  
  
    

}

public void controlEvent(ControlEvent event) {
  if (event.isController()) {
    if (event.getController().getName().equals("buggy ON")) {
      println("Buggy ON button pressed");
      myclient.write("w");
    }
    if (event.getController().getName().equals("buggy OFF")) {
      println("Buggy OFF button pressed");
      myclient.write("s");
    }
  
  }
  
  if(event.isGroup() && event.getName().equals("CruiseModes")) {
    println("Cruising at: " , event.getValue());
    myclient.write("event.getValue()"); 
    
  }
  
  String inputText = textbox.getText();
  if (inputText.equals("go")) {
    println("Buggy ON button pressed");
    myclient.write("w");
  } else if (inputText.equals("stop")) {
    println("Buggy OFF button pressed");
   myclient.write("s");
  } 
  
  switch (inputText) {
     case "c" : 
     println("Cruise Mode");
     myclient.write("c"); 
     break;
     case "p" :
     println("PID mode");
     myclient.write("p");
     break;
   }  
 
}
