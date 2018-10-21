import processing.serial.*;
import org.gicentre.utils.stat.*;
JSONObject json;
Serial myPort;        // The serial port
float lastFrameY, lastL;
XYChart lineChart;
void setup () 
{
  size(640, 640);
  background(0);
  stroke(255);
  
  println(Serial.list());
  myPort = new Serial(this, "COM5", 9600);
  
  json = new JSONObject();
  // Both x and y data set here.  
  lineChart = new XYChart(this);
  lineChart.draw(15,15,width-30,height-30);
}
void draw () 
{
  if (myPort.available() > 0)
  {
      String inString = myPort.readStringUntil(10);
      if (inString != null) {
        println(inString);
        json = parseJSONObject(inString);
        if (json == null) {
          println("JSONObject could not be parsed");
        } else {
          int l = json.getInt("l");
          lineChart.setData(frameCount, l);
          //line(frameCount-1, 100-lastL  , frameCount, 100-l);
          //lastFrameY = frameRate;
          //lastL = l;
        }
      }
  }
}
 
