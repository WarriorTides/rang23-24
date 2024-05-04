
#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH THE MAC Address of your receiver
// float one 80:7D:3A:49:34:95

// loose one 8C:AA:B5:4F:F1:4A
//  uint8_t broadcastAddress[] = {0xFC, 0xF5, 0xC4, 0x91, 0xA4, 0x86};//Topside
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x93, 0x96, 0x34};

String input = "";

// Updates DHT readings every 10 seconds
const long interval = 10000;
unsigned long previousMillis = 0; // will store last time DHT was updated

// Variable to store if sending data was successful
String success;

// Structure example to send data
// Must match the receiver structure
typedef struct control_message
{
  char c;
  int val;
} control_message;

typedef struct reciv_message
{
  int p[20];
  // int d[120];
  int t[20];
} reciv_message;

// Create a struct_message called DHTReadings to hold sensor readings
control_message ControlData;

// Create a struct_message to hold incoming sensor readings
reciv_message recivReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.println();
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0)
  {
    Serial.println("Delivery success");
  }
  else
  {
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
int lasttime=0;
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  
  memcpy(&recivReadings, incomingData, sizeof(recivReadings));
  Serial.print("Bytes received: ");
  lasttime=0;
  Serial.println(len);

  // Serial.print("Depth: ");
  // printIntArray(recivReadings.d);
  for(int i =0; i<20; i++){
    if(lasttime>recivReadings.t[0]){
      break;
    }
    else{
      lasttime=recivReadings.t[0];
      Serial.print("DATA:  Team: RNO6 Pressure: ");
      Serial.print(float(recivReadings.p[0])/1000);
      Serial.print("Kpa Local Time:");
      Serial.println(lasttime);
      Serial.println("s");


    }
  }
  printIntArray(recivReadings.p);

  Serial.print("   Time: ");
  printIntArray(recivReadings.t);
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
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

void loop()
{

  if (Serial.available() > 0)
  {
    input = Serial.readStringUntil('\n');
    int index = input.indexOf('/');
    ControlData.c = input.substring(0, index).charAt(0);
    ControlData.val = (input.substring(index + 1)).toInt();
    Serial.print(ControlData.c);
    Serial.print(ControlData.val);
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *)&ControlData, sizeof(ControlData));
  }
  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
  //   // save the last time you updated the DHT values
  //   previousMillis = currentMillis;

  // }
}

void printIntArray(int arr[])
{
  for (int i = 0; i < 10; i++)
  {
    Serial.print(arr[i]);
    Serial.print(" "); // Optional: Adds a space between elements for readability

  }
}
