
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include "MS5837.h"
#define IN1_PIN D5  // Motor control pin 1 (D5)
#define IN2_PIN D7  // Motor control pin 2 (D7)

MS5837 sensor;

// REPLACE WITH THE MAC Address of your receiver 
//breadbordone=F4:CF:A2:DF:88:65
//loose one 50:02:91:7B:86:6B

// uint8_t broadcastAddress[] = {0xF4, 0xCF, 0xA2, 0xDF, 0x88, 0x65};//breadboard
uint8_t broadcastAddress[] = {0x50, 0x02, 0x91, 0x7B, 0x86, 0x6B};//loose



// Digital pin connected to the DHT sensor



// Define variables to store DHT readings to be sent

// Define variables to store incoming readings
int p[10];
int d[10];
int t[10]; 
// Updates DHT readings every 10 seconds
const long interval = 1000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 
 long floatDur = 0; 
unsigned long previousMillisFloat = 0;    // will store last time DHT was updated 
bool floatIsStopped=true;

int datacount=0;
// Variable to store if sending data was successful
String success;

typedef struct control_message {
    char c;
    int val;
} control_message;

typedef struct send_message {
    int p[10];
int d[10];
int t[10]; 
} send_message;


// Create a struct_message called DHTReadings to hold sensor readings
control_message ControlData;

// Create a struct_message to hold incoming sensor readings
send_message sendReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&ControlData, incomingData, sizeof(ControlData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("COmmand: ");
  Serial.print(ControlData.c);
  Serial.print(" Time:");
  Serial.println(ControlData.val);
  floatDur=ControlData.val;
  previousMillisFloat=millis();
  if(ControlData.c =='f'){
    forward();
    
    floatIsStopped=false;
  }

  else if(ControlData.c =='b'){
    back();
    floatIsStopped=false;
   }
  else if(ControlData.c =='s'){
    stop();
    floatIsStopped=true;

  }
}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
   // Set motor control pins as output
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  stop();
  Wire.begin();

  // Initialize pressure sensor
  // Returns true if initialization was successful
  // We can't continue with the rest of the program unless we can initialize the sensor
  while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }

  // .init sets the sensor model for us but we can override it if required.
  // Uncomment the next line to force the sensor model to the MS5837_30BA.
  //sensor.setModel(MS5837::MS5837_30BA);

  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis-previousMillis>=floatDur && !floatIsStopped){
    stop();
  }
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
      sensor.read();

    p[datacount]=int(round(sensor.pressure()*100));
    d[datacount]=int(round(sensor.depth()*100));
    t[datacount]=millis();
    datacount++;

    if(datacount==10){
      datacount=0;
      for (int i = 0; i < 10; i++) {
    sendReadings.d[i] = d[i];
    sendReadings.p[i] = p[i];
    sendReadings.t[i] = t[i];
}
printIntArray(sendReadings.d);
      esp_now_send(broadcastAddress, (uint8_t *) &sendReadings, sizeof(sendReadings));
    }


  }
}
void printIntArray(int arr[]) {
 for (int i = 0; i < 10; i++) {
    Serial.print(arr[i]);
    Serial.print(" "); // Optional: Adds a space between elements for readability
 }
 Serial.println(); // Moves to the next line after printing all elements
}
void stop(){
  Serial.println("stpoing");
    digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  floatIsStopped=true;

}

void back(){
    Serial.println("back");

   digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
}
void forward(){
      Serial.println("forward");

   digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}