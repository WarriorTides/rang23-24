#include <ESP8266WiFi.h>
//float one 8C:AA:B5:4F:F1:4A

//loose one EC:FA:BC:C9:D8:D8


void pln(String x){
  Serial.println(x);
}
void p(String x){
  Serial.print(x);
}
void setup(){
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println(WiFi.macAddress());
  Serial.println();
}
 
void loop(){
 yield();
}