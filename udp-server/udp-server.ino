

#include <UIPEthernet.h>
#include "utility/logging.h"
#include <Servo.h>
EthernetUDP udp;
Servo t1;
Servo t2;
Servo t3;
Servo t4;
Servo t5;
Servo t6;
Servo t7;
Servo t8;
Servo s1;
Servo s2;
Servo s3;

void setup()
{
    Serial.begin(9600);
    t1.attach(6);
    t2.attach(8);
    t3.attach(10);
    t4.attach(12);
    t5.attach(2);
    t6.attach(4);
    t7.attach(14);
    t8.attach(16);
    s1.attach(9);
    s2.attach(5);
    s3.attach(7);

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

            Serial.print(("received: "));
            Serial.println(msg);
            char command = msg[0];
            String data = String(msg).substring(2);
            Serial.print("Command: ");
            Serial.println(command);

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
                        Serial.println(output[i]);
                    }
                    else
                    {
                        output[i] = data.substring(0, index).toInt();
                        Serial.println(output[i]);
                        data = data.substring(index + 1);
                        i++;
                    }
                }
                // write to thrusters
                t1.writeMicroseconds(output[0]);
                t2.writeMicroseconds(output[1]);
                t3.writeMicroseconds(output[2]);
                t4.writeMicroseconds(output[3]);
                t5.writeMicroseconds(output[4]);
                t6.writeMicroseconds(output[5]);
                t7.writeMicroseconds(output[6]);
                t8.writeMicroseconds(output[7]);
                s1.write(output[8]);
                s2.write(output[9]);
                s3.write(output[10]);
            }

            free(msg);
        } while ((size = udp.available()) > 0);
        // finish reading this packet:
        udp.flush();

        int success;
        do
        {

            Serial.print(("remote ip: "));

            Serial.println(udp.remoteIP());

            Serial.print(("remote port: "));
            Serial.println(udp.remotePort());

            // send new packet back to ip/port of client. This also
            // configures the current connection to ignore packets from
            // other clients!
            success = udp.beginPacket(udp.remoteIP(), udp.remotePort());

            Serial.print(("beginPacket: "));
            Serial.println(success ? "success" : "failed");

            // beginPacket fails if remote ethaddr is unknown. In this case an
            // arp-request is send out first and beginPacket succeeds as soon
            // the arp-response is received.
        } while (!success);

        success = udp.println(F("hello world from arduino"));

        Serial.print(("bytes written: "));
        Serial.println(success);

        success = udp.endPacket();

        Serial.print(("endPacket: "));
        Serial.println(success ? "success" : "failed");

        // udp.stop();
        // // restart with new connection to receive packets from other clients
        // success = udp.begin(5000);

        // Serial.print(("restart connection: "));
        // Serial.println(success ? "success" : "failed");
    }
}