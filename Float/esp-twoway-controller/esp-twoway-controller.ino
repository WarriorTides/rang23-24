
#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x8C, 0xAA, 0xB5, 0x4F, 0xF1, 0x4A};//flaot
// uint8_t broadcastAddress[] = {0x80, 0x7D, 0x3A, 0x49, 0x34, 0x95};//topside

String input ="";

// Updates DHT readings every 10 seconds
const long interval = 10000; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct control_message {
    char c;
    int val;
} control_message;

typedef struct reciv_message {
    int p[10];
int d[10];
int t[10]; 
} reciv_message;

// Create a struct_message called DHTReadings to hold sensor readings
control_message ControlData;

// Create a struct_message to hold incoming sensor readings
reciv_message recivReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println();
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
  memcpy(&recivReadings, incomingData, sizeof(recivReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("Depth: ");
  printIntArray(recivReadings.d);
  Serial.print("   Preassure: ");
  printIntArray(recivReadings.p);
  
  Serial.print("   Time: ");
  printIntArray(recivReadings.t);

}



void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

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
  Serial.println("ME GOT SETUP");
}
 
void loop() {
  
   if (Serial.available() > 0) {
      input = Serial.readStringUntil('\n');
      int  index = input.indexOf('/');
      ControlData.c=input.substring(0, index).charAt(0);
    ControlData.val=(input.substring(index+1)).toInt();
    Serial.print(ControlData.c);
    Serial.print(ControlData.val);
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &ControlData, sizeof(ControlData));


    }
  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
  //   // save the last time you updated the DHT values
  //   previousMillis = currentMillis;

  


  // }
}

void printIntArray(int arr[]) {
 for (int i = 0; i < 10; i++) {
    Serial.print(arr[i]);
    Serial.print(" "); // Optional: Adds a space between elements for readability
 }
}