
#include <UIPEthernet.h>
#include "utility/logging.h"
#include "MS5837.h"
#include <Wire.h>
#include <Servo.h>
#include <PID_v1.h>

typedef struct dataStorage
{
    int upThrusters[4];
    float p;
    float i;
    float d;
    int servoAngles[3];
    bool initialized;
} dataStorage;
dataStorage datastore;

EthernetUDP udp;
Servo thrusters[8];
const byte thrusterPins[] = {6, 8, 10, 12, 2, 4, 14, 16};
const byte servoPins[] = {9, 5, 7};
// cosnt
Servo servos[3];

String sendData = "";
bool runpid = false;
MS5837 depthSensor;
double depthInput, depthOutput;
double depthSetpoint = -1;
PID depthPID(&depthInput, &depthOutput, &depthSetpoint, datastore.p, datastore.i, datastore.d, DIRECT);

void setup()
{
    Wire.begin();
    depthSensorSetup();
    Serial.begin(9600);

    // Set default values
    datastore.upThrusters[0] = 0;
    datastore.upThrusters[1] = 1;
    datastore.upThrusters[2] = 2;
    datastore.upThrusters[3] = 3;
    datastore.p = 1.0;
    datastore.i = 0.1;
    datastore.d = 0.01;
    datastore.servoAngles[0] = 180;
    datastore.servoAngles[1] = 159;
    datastore.servoAngles[2] = 17;
    datastore.initialized = true; // Mark the struct as initialized

    for (int i = 0; i < 8; i++)
    {
        thrusters[i].attach(thrusterPins[i]);
        thrusters[i].writeMicroseconds(1500);
    }
    for (int i = 0; i < 3; i++)
    {
        servos[i].attach(servoPins[i]);
        servos[i].write(datastore.servoAngles[i]);
    }
    Serial.println(datastore.upThrusters[0]);
    Serial.println(datastore.servoAngles[0]);
    Serial.println(datastore.p);

    uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    Ethernet.begin(mac, IPAddress(192, 168, 1, 151));

    int success = udp.begin(8888);

    Serial.print("initialize: ");
    Serial.println(success ? "success" : "failed");
    Serial.println(Ethernet.localIP());
    depthPID.SetOutputLimits(-200, 200);
    depthPID.SetMode(AUTOMATIC);
}
const long interval = 1000;
unsigned long previousMillis = 0; //

void loop()
{

    // check for new udp-packet:
    int size = udp.parsePacket();
    if (size > 0)
    {
        do
        {
            char *msg = (char *)malloc(size + 1);
            int len = udp.read(msg, size + 1);
            msg[len] = 0;

            depthSensor.read();
            depthInput = depthSensor.depth();

            // Serial.print(("received: "));
            Serial.println(msg);
            char command = msg[0];
            String data = String(msg).substring(2);
            sendData = String(msg).substring(2) + "dd" + String(depthInput);
            // Serial.print("Command: ");
            // Serial.println(command);
            if (command == 'c')
            {
                runpid = false;
                // Convert String into an Int Array that contains microseconds for all 8 thrusters and Servo Angles
                int output[11];
                boolean done = false;
                int i = 0;
                while (!done)
                {
                    int index = data.indexOf(',');
                    if (index == -1)
                    {
                        done = true;
                        output[i] = data.toInt();
                        // Serial.println(output[i]);
                    }
                    else
                    {
                        output[i] = data.substring(0, index).toInt();
                        // Serial.println(outputx[i]);
                        data = data.substring(index + 1);
                        i++;
                    }
                }
                // write to thrusters
                for (int i = 0; i < 8; i++)
                {
                    thrusters[i].writeMicroseconds(output[i]);
                }
                // write to servos
                for (int i = 0; i < 3; i++)
                {
                    servos[i].write(output[i + 8]);
                }
            }
            else if (command == 'p')
            {
                datastore.p = data.substring(0, data.indexOf(',')).toFloat();
                datastore.i = data.substring(data.indexOf(',') + 1, data.lastIndexOf(',')).toFloat();
                datastore.d = data.substring(data.lastIndexOf(',') + 1, data.indexOf('d')).toFloat();
                depthSetpoint = data.substring(data.indexOf('d') + 1).toFloat();
                runpid = false;
                depthPID.SetTunings(datastore.p, datastore.i, datastore.d);
            }
            else if (command == 't')
            {

                for (int i = 0; i < 4; i++)
                {
                    datastore.upThrusters[i] = data.substring(i * 2, i * 2 + 1).toInt();
                }
            }
            free(msg);
        } while ((size = udp.available()) > 0);
        // finish reading this packet:
        udp.flush();

        int success;
        do
        {

            // Serial.print(("remote ip: "));

            // Serial.println(udp.remoteIP());

            // Serial.print(("remote port: "));
            // Serial.println(udp.remotePort());

            // send new packet back to ip/port of client. This also
            // configures the current connection to ignore packets from
            // other clients!
            success = udp.beginPacket(udp.remoteIP(), udp.remotePort());

            // Serial.print(("beginPacket: "));
            // Serial.println(success ? "success" : "failed");

            // beginPacket fails if remote ethaddr is unknown. In this case an
            // arp-request is send out first and beginPacket succeeds as soon
            // the arp-response is received.
        } while (!success);
        success = udp.println(sendData);

        // Serial.print(("bytes written: "));
        // Serial.println(success);

        success = udp.endPacket();

        // Serial.print(("endPacket: "));
        // Serial.println(success ? "success" : "failed");

        // udp.stop();
        // // restart with new connection to receive packets from other clients
        // success = udp.begin(5000);

        // Serial.print(("restart connection: "));
        // Serial.println(success ? "success" : "failed");
    }
    if (millis() - previousMillis >= interval)
    {
        previousMillis = millis();
        if (runpid)
        {
            Serial.println("Running PID");
            depthSensor.read();
            depthInput = depthSensor.depth();
            Serial.print("DepthInput:");
            Serial.print((depthInput));
            Serial.print(",");
            Serial.println("pwmwriting:" + String(depthOutput + 1500));
            depthPID.Compute();
            thrusters[datastore.upThrusters[0]].writeMicroseconds(depthOutput + 1500);
            thrusters[datastore.upThrusters[0]].writeMicroseconds(depthOutput + 1500);
            thrusters[datastore.upThrusters[0]].writeMicroseconds(depthOutput + 1500);
            thrusters[datastore.upThrusters[0]].writeMicroseconds(depthOutput + 1500);
        }
    }
}
void depthSensorSetup()
{
    depthSensor.setModel(MS5837::MS5837_02BA);
    depthSensor.init();
    depthSensor.setFluidDensity(997); // kg/m^3 (997 freshwater, 1029 for seawater)
}
