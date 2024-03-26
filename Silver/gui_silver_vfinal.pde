import controlP5.*;
import processing.net.*;
//import meter.*;

int distance =   0;

ControlP5 p5;
Button GOButton;
Button STOPButton;
Textfield refSpeedTextbox;
Knob refSpeedKnob;
Client myclient;
PFont font, pFont;
String data, obs_seen = "clear", control_string = "Control mode: distance PID.", distance_string = "Distance travelled: ", bs_string = "Buggy speed: ", us_string = "Distance to buggy: 0cm";
boolean control_mode = true;


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
 
  //refSpeedTextbox = p5.addTextfield("ref Speed")
  //             .setPosition(400, 600)
  //             .setSize(400, 80)
  //             .setAutoClear(true)
  //             .setFont(createFont("calibri", 40))
  //             .setColorBackground(color(255, 255, 255))
  //             .setColor(color(0));
  
  refSpeedKnob = p5.addKnob("Speed")
                 .setPosition(400, 600)
                .setRange(8, 15)
                .setRadius(150)
                .setDragDirection(Knob.VERTICAL);
                
               
               
               
            
  myclient = new Client(this, "192.168.4.1", 5200);
  myclient.write("connecting");
  // code to connect to arduino
  font = createFont("calibri", 25);
}

void draw() {
  background(0);
  
  textFont(pFont,40);                 
  fill(255);                         
  text(bs_string,900,250);
  
  textFont(pFont,40);                 
  fill(255);                         
  text("Obstruction: "+obs_seen,900,550);
  
  textFont(pFont,40);                 
  fill(255);                         
  text(control_string,900,850);
  
  textFont(pFont,40);                 
  fill(255);                         
  text(distance_string,900,675);
  
  textFont(pFont,40);                 
  fill(255);                         
  text(us_string,900,375);
  
  
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
      obs_seen = "Clear.";
    }
    
    if(data.equals("O")){
      obs_seen = "Obstruction!";
      
    }
    
    if(data.startsWith("d")){
      distance_string = "Distance travelled: " + data.substring(1) + "cm";
      
    }
    
    if(data.startsWith("bs")){
      bs_string = "Buggy speed: " + data.substring(2) + "cm/s";
      
    }
    
    if(data.startsWith("us")){
      us_string = "Distance to buggy: " + data.substring(2) + "cm";
      
    }
    
    if(data.equals("01")){
      control_string = "Control mode: reference speed.";
    }else if(data.equals("00")){
      control_string = "Control mode: distance PID.";
    }
    
    //if (data.equals("d")){ 
    //  String distance_travelled = myclient.readString();
    //  distance_string = "Distance travelled: " + distance_travelled;
    //}
    
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
    if(event.getController().getName().equals("Speed")){
      float sliderValue = event.getController().getValue();
      println(int(sliderValue));
      myclient.write('n');
      String to_send = sliderValue + "n";
      myclient.write(to_send);
    }
    
    
    //if (event.getController().getName().equals("ref Speed")) {
      
    //  String refSpeedString = refSpeedTextbox.getText();
    //  if (refSpeedString.length() > 0) {
    //    println("Sending reference speed:", refSpeedString);
    //    myclient.write(refSpeedString);
    //  }

    }
  }
 
 
