#include <ESP8266WiFi.h>
//breadbnoard one F4:CF:A2:DF:88:65
//loose one 50:02:91:7B:86:6B

void setup(){
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}