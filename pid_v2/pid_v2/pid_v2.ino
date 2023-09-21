//Depth PID added to control system code
#include<Servo.h>
#include "MS5837.h"
#include <PID_v1.h>
#include <Wire.h>
// 1369.10
// 17:13:19.543 -> i 0.20
// 17:13:19.543 -> d 2338975.80
 double dKp = 1168.75, dKi = 0.16, dKd =2190531.80;
double  depthInput, depthOutput;
double depthSetpoint=1;

PID depthPID(&depthInput, &depthOutput, &depthSetpoint, dKp, dKi, dKd, DIRECT);

Servo top_front;
Servo top_back;



MS5837 depthSensor;

void setup() {
    Wire.begin();
    Serial.begin(9600);
    depthPID.SetOutputLimits(-100, 100);
    depthPID.SetMode(AUTOMATIC);
    depthSensorSetup();
    motorSetup();
    Serial.println("ready");
}
long microseconds;

void loop() {
          long prevMicroseconds = microseconds;
        microseconds = micros();
    depthSensor.read();
    depthInput = depthSensor.depth();
    depthPID.Compute();
    Serial.println("depth input " + String(depthInput) + " output " + String(depthOutput) + " setpoint " +
                   String(depthSetpoint));
    
    
    depthPID.SetMode(AUTOMATIC);
    top_back.writeMicroseconds(depthOutput + 1500);
    top_front.writeMicroseconds(depthOutput + 1500);
    // delay();
            while (micros() - microseconds < 250) delayMicroseconds(1);

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