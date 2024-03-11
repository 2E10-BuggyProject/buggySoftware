import controlP5.*;
import processing.net.*;
//import meter.*;

ControlP5 p5;
Button GOButton;
Button STOPButton;
Textfield textbox;
Textarea textoutput; 
Println console;
Slider slider;
Client myclient;
PFont font;
String data;



void setup() {
  size(400, 500);
  smooth();
  noStroke();
  
  p5 = new ControlP5(this);
 
  GOButton = p5.addButton("POWER ON")
               .setValue(100)
               .setPosition(50,20)
               .setSize(100,100);
 
  STOPButton = p5.addButton("POWER OFF")
                 .setValue(0)
                 .setPosition(170,20)
                 .setSize(100,100);
 
  textbox = p5.addTextfield("COMMAND CENTRE")
               .setPosition(50, 140)
               .setSize(220, 50)
               .setAutoClear(true)
               .setFont(createFont("calibri", 20))
               .setColorBackground(color(255, 255, 255))
               .setColor(color(0));
               
 slider = p5.addSlider("SPEED")
            .setPosition(50,240)
            .setSize(220,50)
            .setRange(0,1)
            .setValue(1);
            
 
 textoutput = p5.addTextarea("CONSOLE")
               .setPosition(50, 340)
               .setSize(220, 80)
              // .setAutoClear(true)
               .setFont(createFont("calibri", 20))
               .setColorBackground(color(255, 255, 255))
               .setColor(color(0));             
  
 console = p5.addConsole(textoutput);   

  
            
  myclient = new Client(this, "192.168.4.1", 5200);
  //myclient.write("connecting");
  // code to connect to arduino
  background(0,0,0);
  
}

void draw() {
background(0); 
 
  //text("Y1 GUI", 50, 80);
  // for aduino
  data = myclient.readString();
if (data!=null) {
 println(data);
}
 delay(1000);
 
}

public void controlEvent(ControlEvent event) {
  if (event.isController()) {
    if (event.getController().getName().equals("POWER ON")) {
      println("POWER ON button pressed");
      myclient.write("go");
    }
    if (event.getController().getName().equals("POWER OFF")) {
      println("POWER OFF button pressed");
      myclient.write("stop");
    }
    if (event.getController().getName().equals("SPEED")) {
    println(this.slider.getValue());
    data = "this.slider.getValue()"; 
    myclient.write(data); 
}
  }

  String inputText = textbox.getText();
  if (inputText.equals("go")) {
    println("Buggy ON button pressed");
    myclient.write("go");
  } else if (inputText.equals("stop")) {
    println("Buggy OFF button pressed");
   myclient.write("stop");
  } 
 
}
