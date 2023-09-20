//Depth PID added to control system code
#include<Servo.h>
#include "MS5837.h"
#include <PID_v1.h>
#include <Wire.h>

double dKp = 2, dKi = 5, dKd = 1;

double depthSetpoint, depthInput, depthOutput;

PID depthPID(&depthInput, &depthOutput, &depthSetpoint, dKp, dKi, dKd, DIRECT);
int FRV_pin = 6, FLV_pin = 2, BRV_pin = 46, BLV_pin = 5, FRH_pin = 4, FLH_pin = 2, BRH_pin = 7, BLH_pin = 3;
Servo top_front;
Servo top_back;

// Pin definitions
int myLed = 13;  // Set up pin 13 led for toggling
MS5837 depthSensor;

void setup() {
    Wire.begin();
    Serial.begin(9600);
    depthPID.SetOutputLimits(-400, 400);
    depthPID.SetMode(AUTOMATIC);
    depthSensorSetup();
    motorSetup();
    Serial.println("ready");
}

void loop() {
    depthSensor.read();
    depthInput = depthSensor.depth();
    depthPID.Compute();
    Serial.println("depth input " + String(depthInput) + " output " + String(depthOutput) + " setpoint " +
                   String(depthSetpoint));
    String input = Serial.readStringUntil('x');
    if (input.substring(0, input.length() - 2).equals("open-claw")) {
        claw.writeMicroseconds(0);
    }
    if (input.substring(0, input.length() - 2).equals("close-claw")) {
        claw.writeMicroseconds(255);
    }
    int dir = input.substring(0, 1).toInt();
    int thruster_value = input.substring(2, input.length() - 1).toFloat() * 100;

    Serial.println("direction " + String(dir) + " thruster_value " + String(thruster_value));
    //sway
    if (dir == 0) {
        //  Serial.println("writing values to FRH, BRH, FLH, BLH");
        front_right_vertical.writeMicroseconds(1500);
        back_right_vertical.writeMicroseconds(1500);
        front_left_vertical.writeMicroseconds(1500);
        back_left_vertical.writeMicroseconds(1500);

        front_right_horizontal.writeMicroseconds(thruster_value * -1 + 1500);
        back_right_horizontal.writeMicroseconds(thruster_value + 1500);
        front_left_horizontal.writeMicroseconds(thruster_value + 1500);
        back_left_horizontal.writeMicroseconds(thruster_value * -1 + 1500);

    }
        //surge
    else if (dir == 1) {
        front_right_vertical.writeMicroseconds(1500);
        back_right_vertical.writeMicroseconds(1500);
        front_left_vertical.writeMicroseconds(1500);
        back_left_vertical.writeMicroseconds(1500);

        front_right_horizontal.writeMicroseconds(thruster_value + 1500);
        front_left_horizontal.writeMicroseconds(thruster_value + 1500);
        back_right_horizontal.writeMicroseconds(thruster_value + 1500);
        back_left_horizontal.writeMicroseconds(thruster_value + 1500);
    }
        //heave
    else if (dir == 3) {
        depthPID.SetMode(MANUAL);
        front_right_horizontal.writeMicroseconds(1500);
        back_right_horizontal.writeMicroseconds(1500);
        front_left_horizontal.writeMicroseconds(1500);
        back_left_horizontal.writeMicroseconds(1500);

        front_right_vertical.writeMicroseconds(thruster_value + 1500);
        front_left_vertical.writeMicroseconds(thruster_value + 1500);
        back_right_vertical.writeMicroseconds(thruster_value + 1500);
        back_left_vertical.writeMicroseconds(thruster_value + 1500);
    }
        //yaw
    else if (dir == 4) {
        front_right_vertical.writeMicroseconds(1500);
        back_right_vertical.writeMicroseconds(1500);
        front_left_vertical.writeMicroseconds(1500);
        back_left_vertical.writeMicroseconds(1500);

        front_right_horizontal.writeMicroseconds(thruster_value + 1500);
        back_right_horizontal.writeMicroseconds(thruster_value + 1500);
        front_left_horizontal.writeMicroseconds(-1 * thruster_value + 1500);
        back_left_horizontal.writeMicroseconds(-1 * thruster_value + 1500);
    }
    depthPID.SetMode(AUTOMATIC);
    front_right_vertical.writeMicroseconds(depthOutput + 1500);
    front_left_vertical.writeMicroseconds(depthOutput + 1500);
    back_right_vertical.writeMicroseconds(depthOutput + 1500);
    back_left_vertical.writeMicroseconds(depthOutput + 1500);

}


void depthSensorSetup() {
    depthSensor.setModel(MS5837::MS5837_02BA);
    depthSensor.init();
    depthSensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
}

void motorSetup() {
  top_front = 5;
  top_back = 2;
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