#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055(0x)

void setup() {
  Serial.begin(9600);

  // Initialize the BNO08x
  if (!bno.begin()) {
    Serial.println("Failed` to initialize BNO08x");
    while (1);
  }

  Serial.println("BNO08x initialized successfully!");
}

void loop() {
  // Get the current orientation
  sensors_event_t event;
  bno.getEvent(&event);

  // Print the current orientation to the serial monitor
  Serial.print("Orientation: ");
  Serial.print(event.orientation.x);
  Serial.print(", ");
  Serial.print(event.orientation.y);
  Serial.print(", ");
  Serial.println(event.orientation.z);

  // Delay for 100 milliseconds
  delay(100);
}