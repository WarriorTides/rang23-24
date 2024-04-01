#include <EEPROM.h>

#include <UIPEthernet.h>
#include "utility/logging.h"
#include <Servo.h>

EthernetUDP udp;
Servo thrusters[8];
const byte thrusterPins[] = {6, 8, 10, 12, 2, 4, 14, 16};
const byte servoPins[] = {9, 5, 7};

Servo servos[3];

String sendData = "";

typedef struct dataStorage
{
    int[4] upThrusters;
    float p;
    float i;
    float d;
    int[3] servoAngles;
} dataStorage;
dataStorage data;

void setup()
{
    Serial.begin(9600);

    EEPROM.get(0, data); // Attempt to retrieve the struct from EEPROM

    // Check if the struct has been initialized
    if (!data.initialized)
    {
        // Set default values
        data.upThrusters[0] = 0;
        data.upThrusters[1] = 1;
        data.upThrusters[2] = 2;
        data.upThrusters[3] = 3;
        data.p = 1.0;
        data.i = 0.1;
        data.d = 0.01;
        data.servoAngles[0] = 180;
        data.servoAngles[1] = 159;
        data.servoAngles[2] = 17;
        data.initialized = true; // Mark the struct as initialized

        EEPROM.put(0, data); // Store the struct with default values
        EEPROM.commit();     // Write all changes to EEPROM
    }

    for (int i = 0; i < 8; i++)
    {
        thrusters[i].attach(thrusterPins[i]);
        thrusters[i].writeMicroseconds(1500);
    }
    for (int i = 0; i < 3; i++)
    {
        servos[i].attach(servoPins[i]);
        servos[i].write(data.servoAngles[i]);
    }

    uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    Ethernet.begin(mac, IPAddress(192, 168, 1, 151));

    int success = udp.begin(8888);

    Serial.print("initialize: ");
    Serial.println(success ? "success" : "failed");
    Serial.println(Ethernet.localIP());
}

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

            // Serial.print(("received: "));
            Serial.println(msg);
            char command = msg[0];
            String data = String(msg).substring(2);
            sendData = String(msg).substring(2);
            // Serial.print("Command: ");
            // Serial.println(command);

            if (command == 'c')
            {
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
                data.p = data.substring(0, data.indexOf(',')).toFloat();
                data.i = data.substring(data.indexOf(',') + 1, data.lastIndexOf(',')).toFloat();
                data.d = data.substring(data.lastIndexOf(',') + 1).toFloat();
                EEPROM.put(0, data);
            }
            else if (command == 't')
            {

                for (int i = 0; i < 4; i++)
                {
                    data.upThrusters[i] = data.substring(i * 2, i * 2 + 1).toInt();
                }
                EEPROM.put(0, data);
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
}