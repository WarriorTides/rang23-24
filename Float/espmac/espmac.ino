#include <ESP8266WiFi.h>
//float one F4:CF:A2:DF:88:65
//loose one E0:98:06:A8:7A:C0


void setup(){
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}