//Depth PID added to control system code
#include<Servo.h>
#include "MS5837.h"
#include <PID_v1.h>
#include <Wire.h>

double dKp = 500, dKi = 5, dKd = 1;
double  depthInput, depthOutput;
double depthSetpoint=0.5;

PID depthPID(&depthInput, &depthOutput, &depthSetpoint, dKp, dKi, dKd, DIRECT);

Servo top_front;
Servo top_back;



MS5837 depthSensor;

void setup() {
    Wire.begin();
    Serial.begin(9600);
    depthPID.SetOutputLimits(-200, 200);
    depthPID.SetMode(AUTOMATIC);
    depthSensorSetup();
    motorSetup();
    Serial.println("ready");
}
long microseconds;

void loop() {
if (Serial.available()) {
    float incomingValue = Serial.parseFloat();
    if (incomingValue!=0){
    depthSetpoint = incomingValue;
    }
    // Do something with the float value
  } else {
    long prevMicroseconds = microseconds;
    microseconds = micros();
    depthSensor.read();
    depthInput = depthSensor.depth();
    depthPID.Compute();
    Serial.print("DepthInput:" );
    Serial.print( (depthInput) );
    Serial.print("," );
    Serial.print("Setpoint:" );
    Serial.println( (depthSetpoint) );
    // if()
    // float floatVariable = Serial.parseFloat();
    // Serial.println(Serial.readStringUntil("/n"));


    
    depthPID.SetMode(AUTOMATIC);
    top_back.writeMicroseconds(depthOutput + 1500);
    top_front.writeMicroseconds(depthOutput + 1500);
    // delay();
            while (micros() - microseconds < 250) delayMicroseconds(1);
  }

}


void depthSensorSetup() {
    depthSensor.setModel(MS5837::MS5837_02BA);
    depthSensor.init();
    depthSensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
}

void motorSetup() {
  top_front.attach(5);
  top_back.attach(2);
  top_front.writeMicroseconds(1500);
  top_back.writeMicroseconds(1500);
  delay(7000);
}
// void motorSetup(){
//     front_right_vertical.attach(FRV_pin);
//     front_left_vertical.attach(FLV_pin);
//     back_right_vertical.attach(BRV_pin); //4 and  46
//     back_left_vertical.attach(BLV_pin);

//     front_right_horizontal.attach(FRH_pin);
//     front_left_horizontal.attach(FLH_pin);
//     back_right_horizontal.attach(BRH_pin);
//     back_left_horizontal.attach(BLH_pin);

//     front_right_vertical.writeMicroseconds(1500);
//     front_left_vertical.writeMicroseconds(1500);
//     back_right_vertical.writeMicroseconds(1500);
//     back_left_vertical.writeMicroseconds(1500);
//     front_right_horizontal.writeMicroseconds(1500);
//     front_left_horizontal.writeMicroseconds(1500);
//     back_right_horizontal.writeMicroseconds(1500);
//     back_left_horizontal.writeMicroseconds(1500);

//     delay(7000);
// }