

#include <UIPEthernet.h>
#include "utility/logging.h"

EthernetUDP udp;

void setup()
{
    Serial.begin(9600);

    uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

    Ethernet.begin(mac, IPAddress(192, 168, 68, 151));

    int success = udp.begin(8888);

    Serial.print("idddnitialize: ");
    Serial.println(success ? "success" : "failed");
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

            Serial.print(("received: '"));
            Serial.print(msg);

            free(msg);
        } while ((size = udp.available()) > 0);
        // finish reading this packet:
        udp.flush();

        Serial.println(("'"));

        //     int success;
        //     do
        //     {

        //         Serial.print(("remote ip: "));

        //         Serial.println(udp.remoteIP());

        //         Serial.print(("remote port: "));
        //         Serial.println(udp.remotePort());

        //         // send new packet back to ip/port of client. This also
        //         // configures the current connection to ignore packets from
        //         // other clients!
        //         success = udp.beginPacket(udp.remoteIP(), udp.remotePort());

        //         Serial.print(("beginPacket: "));
        //         Serial.println(success ? "success" : "failed");

        //         // beginPacket fails if remote ethaddr is unknown. In this case an
        //         // arp-request is send out first and beginPacket succeeds as soon
        //         // the arp-response is received.
        //     } while (!success);

        //     success = udp.println(F("hello world from arduino"));

        //     Serial.print(("bytes written: "));
        //     Serial.println(success);

        //     success = udp.endPacket();

        //     Serial.print(("endPacket: "));
        //     Serial.println(success ? "success" : "failed");

        //     udp.stop();
        //     // restart with new connection to receive packets from other clients
        //     success = udp.begin(5000);

        //     Serial.print(("restart connection: "));
        //     Serial.println(success ? "success" : "failed");
    }
}
