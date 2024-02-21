import controlP5.*;
import processing.net.*;
//import meter.*;

ControlP5 p5;
Button GOButton;
Button STOPButton;
Textfield textbox;
Slider slider;
Client myclient;
PFont font;
String data;

void setup() {
  size(800, 500);
  p5 = new ControlP5(this);
 
  GOButton = p5.addButton("buggy ON")
               .setValue(100)
               .setPosition(200,100)
               .setSize(200,20);
 
  STOPButton = p5.addButton("buggy OFF")
                 .setValue(0)
                 .setPosition(200,200)
                 .setSize(200,20);
 
  textbox = p5.addTextfield("text")
               .setPosition(200, 300)
               .setSize(200, 40)
               .setAutoClear(true)
               .setFont(createFont("calibri", 20))
               .setColorBackground(color(255, 255, 255))
               .setColor(color(0));
               
 slider = p5.addSlider("slide")
            .setPosition(400,400)
            .setSize(200,50)
            .setRange(0,1)
            .setValue(1);
            
  myclient = new Client(this, "192.168.4.1", 5200);
  myclient.write("connecting");
  // code to connect to arduino
  font = createFont("roman", 15);
}

void draw() {
  background(0);
  textFont(font);
  text("control", 200, 80);
  // fror aduino
  data = myclient.readString();
 println(data);
 delay(1000);
}

public void controlEvent(ControlEvent event) {
  if (event.isController()) {
    if (event.getController().getName().equals("buggy ON")) {
      println("Buggy ON button pressed");
      myclient.write("go");
    }
    if (event.getController().getName().equals("buggy OFF")) {
      println("Buggy OFF button pressed");
      myclient.write("stop");
    }
    if (event.getController().getName().equals("slide")) {
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
