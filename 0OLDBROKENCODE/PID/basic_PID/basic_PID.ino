#include <PID_v1.h>

const int DEPTH_SENSOR_PIN = A0;
const int THRUST_PIN = 9;
const int JOYSTICK_PIN = A1;

double currentDepth;

double setpoint, input, output;
double Kp = 2, Ki = 5, Kd = 1;

// PID myPID(currentDepth, thrusterPower, setpoint, kp, ki, kd, DIRECT);
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);


void setup() {
  myPID.SetOutputLimits(0, 255);

  Serial.begin(115200);

  pinMode(DEPTH_SENSOR_PIN, INPUT);
  pinMode(THRUST_PIN, OUTPUT);

  // myPID.setSetpoint(setpoint);
  // myPID.setOutputLimits(0, 255);

}

void loop() {
  // currentDepth = sensor.depth(DEPTH_SENSOR_PIN); // get this as an input from depth sensor
  currentDepth = 5;

  // if joystick is being used
  if (analogRead(JOYSTICK_PIN) > 500) {
    // myPID.disable();
    Serial.print("****");
  } else {
    // myPID.enable();
    output = myPID.Compute();
    analogWrite(THRUST_PIN, output);
  }


  // for testing purposes
  Serial.print("Current depth: ");
  Serial.println(currentDepth);
  Serial.print("Setpoint: ");
  Serial.println(setpoint);
  Serial.print("Output: ");
  Serial.println(output);

  delay(100);

}
