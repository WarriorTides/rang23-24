

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
Servo servos[11] = {t1, t2, t3, t4, t5, t6, t7, t8, s1, s2, s3};
int servoPins[11] = {6, 8, 10, 12, 2, 4, 14, 16, 9, 5, 7};

void setup()
{
    Serial.begin(9600);
    for (int i = 0; i < 11; i++)
    {
        servos[i].attach(servoPins[i]);
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
            // Serial.println(msg);
            char command = msg[0];

            if (command == 'c')
            {
                // Convert String into an Int Array that contains microseconds for all 8 thrusters and Servo Angles
                int output[11];
                int count = 0;
                msg += 2;
                char *start = msg;
                char *end = msg;

                while (*end)
                {
                    if (*end == ',')
                    {
                        *end = '\0';                   // Temporarily terminate the string
                        output[count++] = atoi(start); // Convert the substring to an integer
                        *end = ',';                    // Restore the comma
                        start = end + 1;               // Move to the next number
                    }
                    end++;
                }
                // Convert the last number
                output[count++] = atoi(start);

                // write to thrusters
                for (int i = 0; i < 11; i++)
                {
                    servos[i].writeMicroseconds(output[i]);
                }
            }
            int success;
            do
            {
                success = udp.beginPacket(udp.remoteIP(), udp.remotePort());
            } while (!success);

            success = udp.println(msg);
            success = udp.endPacket();

            free(msg);
        } while ((size = udp.available()) > 0);
        // finish reading this packet:
        udp.flush();
    }
}
