

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Servo.h>

Servo fr;
Servo fl;
Servo bl;
Servo

    // Enter a MAC address and IP address for your controller below.
    // The IP address will be dependent on your local network:
    byte mac[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 123);

unsigned int localPort = 8888; // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// String[] data;
// unsigned long start_time, end_time, execution_time;

void setup()
{
  // You can use Ethernet.init(pin) to configure the CS pin
  // Ethernet.init(10);  // Most Arduino shields
  // Ethernet.init(5);   // MKR ETH Shield
  // Ethernet.init(0);   // Teensy 2.0
  // Ethernet.init(20);  // Teensy++ 2.0
  // Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet
  // Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet

  // start the Ethernet
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true)
    {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("Ethernet cable is not connected.");
  }
  Serial.println(ip);
  // start UDP
  Udp.begin(localPort);
}

void loop()
{
  // if there's data available, read a packet
  // start_time = micros();

  int packetSize = Udp.parsePacket();
  if (packetSize)
  {

    // Serial.print("Received packet of size ");
    // Serial.println(packetSize);
    // Serial.print("From ");
    // IPAddress remote = Udp.remoteIP();
    // for (int i=0; i < 4; i++) {
    //   Serial.print(remote[i], DEC);
    //   if (i < 3) {
    //     Serial.print(".");
    //   }
    // }
    // Serial.print(", port ");
    // Serial.println(Udp.remotePort());

    // read the packet into packetBuffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    // Serial.println("Contents:");
    Serial.println(packetBuffer);
    String packetString = String(packetBuffer);
    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);

    char command = packetString.charAt(0);
    String data = packetString.substring(2);
    if (command == 'c')
    {
      // Convert String into an Int Array that contains microseconds for all 8 thrusters and Servo Angles
      int output[11];
      boolean done = false;
      int i = 0;
      while (!done)
      {
        int index = data.indexOf('|');
        if (index == -1)
        {
          done = true;
          output[i] = data.toInt();
        }
        else
        {
          output[i] = data.substring(0, index).toInt();
          data = data.substring(index + 1);
          i++;
        }
      }
    }
    else if (command == 's')
    {
      // do something
    }
    else if (command == 'r')
    {
      // do something
    }

    else if (command == 'p')
    {
      // do something
    }
    else if (command == 'd')
    {
      // do something
    }

    // end_time = micros();
    // execution_time = end_time - start_time;

    // Serial.print("Execution time in microseconds: ");
    // Serial.println(execution_time);

    // send a reply to the IP address and port that sent us the packet we received
    // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    // Udp.write(ReplyBuffer);
    // Udp.endPacket();
  }
  delay(10);
}
