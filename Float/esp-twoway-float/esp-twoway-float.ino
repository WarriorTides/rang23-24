
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include "MS5837.h"
#define IN1_PIN D3 // Motor control pin 1 (D5)
#define IN2_PIN D4 // Motor control pin 2 (D7)

MS5837 sensor;

// REPLACE WITH THE MAC Address of your receiver

// float one 8C:AA:B5:16:19:B5
// loose one FC:F5:C4:91:A4:86

uint8_t broadcastAddress[] = {0xFC, 0xF5, 0xC4, 0x91, 0xA4, 0x86}; // Topside
// uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x93, 0x96, 0x34}; //Float

// Digital pin connected to the DHT sensor

// Define variables to store DHT readings to be sent

// Define variables to store incoming readings

const long interval = 5000;            // interval to send readigns
unsigned expandtime = 18000;           // time to expand
long waitTime = 0;                     // wait time between expandinga nd contracitng - set by message sent
unsigned long previousMillisFloat = 0; // timer for float expansion/stopping
bool floatIsStopped = true;
char nextCommand = 's';
bool sendSuccess = false;
int datacount = 0;
int datasendindex = 0;
bool shouldStop = false;
unsigned long previousMillis = 0; // counter used to time readings being sent

// Variable to store if sending data was successful
String success;

typedef struct control_message
{
  char c;
  int val;
} control_message;

typedef struct send_message
{
  int p;
  // int d[120];
  int t;
} send_message;

int preassurReadings[150];
int timeReadings[150];
// Create a struct_message called DHTReadings to hold sensor readings
control_message ControlData;

// Create a struct_message to hold incoming sensor readings
send_message sendReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0)
  {
    Serial.println("Delivery success");

    if (datacount <= datasendindex)
    {
      espNOWSend(datasendindex);
    }
    else
    {
      datacount = 0;
    }
  }
  else
  {
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&ControlData, incomingData, sizeof(ControlData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("COmmand: ");
  Serial.print(ControlData.c);

  Serial.print(" Time:");
  Serial.println(ControlData.val);
  if (ControlData.c == 't')
  {
    expandtime = ControlData.val;
  }
  else
  {
    waitTime = ControlData.val;
    previousMillisFloat = millis();
  }
c:
  \Users if (ControlData.c == 'f')
  {
    forward();
    nextCommand = 'b';
  }

  else if (ControlData.c == 'b')
  {
    back();
    nextCommand = 'f';
  }
  else if (ControlData.c == 's')
  {
    stop();
    nextCommand = 's';
  }
}

void setup()
{
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
  while (!sensor.init())
  {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }

  // .init sets the sensor model for us but we can override it if required.
  // Uncomment the next line to force the sensor model to the MS5837_30BA.
  sensor.setModel(MS5837::MS5837_02BA);

  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)

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
}

void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisFloat >= expandtime && !floatIsStopped)
  {
    stop();
  }

  if (currentMillis - previousMillisFloat >= (waitTime + expandtime))
  {
    if (nextCommand == 'f')
    {
      forward();
      previousMillisFloat = millis();
    }

    else if (nextCommand == 'b')
    {
      back();
      previousMillisFloat = millis();
    }
    else if (nextCommand == 's')
    {
      stop();
    }
    nextCommand = 's';
  }

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    sensor.read();
    // Serial.println(sensor.depth());
    preassurReadings[datacount] = int(round(sensor.depth() * 100));
    // sendReadings.d[datacount] = int(round(sensor.depth() * 100));
    timeReadings[datacount] = int(round(millis() / 100));
    datacount++;

    espNOWSend(0);
  }
}
void printIntArray(int arr[])
{
  for (int i = 0; i < 10; i++)
  {
    Serial.print(arr[i]);
    Serial.print(" "); // Optional: Adds a space between elements for readability
  }
  Serial.println(); // Moves to the next line after printing all elements
}
void stop()
{
  Serial.println("stpoing");
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  floatIsStopped = true;
}

void back()
{
  Serial.println("back");
  floatIsStopped = false;

  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  // shouldStop=true;
  // counterForStoping=millis();
}
void forward()
{
  Serial.println("forward");
  floatIsStopped = false;

  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  // shouldStop=true;
  // counterForStoping=millis();
}

void espNOWSend(int index)
{
  // Copy readings to sendReadings struct

  sendReadings.p = preassurReadings[i];
  // sendReadings.d[i] = depthReadings[i];
  sendReadings.t = timeReadings[i];
  datasendindex++;
  esp_now_send(broadcastAddress, (uint8_t *)&sendReadings, sizeof(sendReadings));
}