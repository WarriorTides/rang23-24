#include <pidautotuner.h>

#include<Servo.h>
#include <Wire.h>
#include "MS5837.h"

MS5837 sensor;

Servo top_front;
Servo top_back;
void setup() {
Wire.begin();
  sensor.setModel(MS5837::MS5837_02BA);
  sensor.init();
  
  sensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
  motorSetup();
  Serial.begin(9600);
    PIDAutotuner tuner = PIDAutotuner();

    // Set the target value to tune to
    // This will depend on what you are tuning. This should be set to a value within
    // the usual range of the setpoint. For low-inertia systems, values at the lower
    // end of this range usually give better results. For anything else, start with a
    // value at the middle of the range.
    tuner.setTargetInputValue(1.0);

    // Set the loop interval in microseconds
    // This must be the same as the interval the PID control loop will run at
    tuner.setLoopInterval(250);

    // Set the output range
    // These are the minimum and maximum possible output values of whatever you are
    // using to control the system (Arduino analogWrite, for example, is 0-255)
    tuner.setOutputRange(1400,1600);

    // Set the Ziegler-Nichols tuning mode
    // Set it to either PIDAutotuner::ZNModeBasicPID, PIDAutotuner::ZNModeLessOvershoot,
    // or PIDAutotuner::ZNModeNoOvershoot. Defaults to ZNModeNoOvershoot as it is the
    // safest option.
    tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);

    // This must be called immediately before the tuning loop
    // Must be called with the current time in microseconds
    tuner.startTuningLoop(micros());

    // Run a loop until tuner.isFinished() returns true
    long microseconds;
    while (!tuner.isFinished()) {

        // This loop must run at the same speed as the PID control loop being tuned
        long prevMicroseconds = microseconds;
        microseconds = micros();
 sensor.read();
        // Get input value here (temperature, encoder position, velocity, etc)
        double input = sensor.depth();


        // Call tunePID() with the input value and current time in microseconds
        double output = tuner.tunePID(input, microseconds);

        // Set the output - tunePid() will return values within the range configured
        // by setOutputRange(). Don't change the value or the tuning results will be
        // incorrect.
        top_back.writeMicroseconds(output);
    top_front.writeMicroseconds(output );
Serial.println("depth input " + String(input) + " output " + String(output));
    
        // This loop must run at the same speed as the PID control loop being tuned
        while (micros() - microseconds < 250) delayMicroseconds(1);
    }

    // Turn the output off here.
        top_back.writeMicroseconds(1500);
            top_front.writeMicroseconds(1500);



    // Get PID gains - set your PID controller's gains to these
    double kp = tuner.getKp();
    double ki = tuner.getKi();
    double kd = tuner.getKd();
    Serial.println("double dKp = "+String(kp)+", dKi = "+String(ki)+", dKd ="+String(kd));
    // Serial.println("i " + String( ki));
    // Serial.println("d " +String( kd));

}

void loop() {

    // ...
}


// void depthSensorSetup() {
//     depthSensor.setModel(MS5837::MS5837_02BA);
//     depthSensor.init();
//     depthSensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
// }

void motorSetup() {
  top_front.attach(5);
  top_back.attach(2);
  top_front.writeMicroseconds(1500);
  top_back.writeMicroseconds(1500);
  delay(7000);
}