#include <EtherCard.h>
#include <IPAddress.h>
#include <Servo.h>

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

#define STATIC 0 // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = {192, 168, 1, 151};
// gateway ip address
static byte gwip[] = {192, 168, 1, 1};
#endif

// ethernet mac address - mu st be unique on your network
static byte mymac[] = {0x70, 0x69, 0x69, 0x2D, 0x30, 0x31};

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

// callback that prints received packets to the serial port
void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *msg, uint16_t len)
{
  IPAddress src(src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  //  Serial.print("dest_port: ");
  //  Serial.println(dest_port);
  //  Serial.print("src_port: ");
  //  Serial.println(src_port);
  //
  //
  //  Serial.print("src_port: ");
  //  ether.printIp(src_ip);
  Serial.println("data: ");
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

  ether.makeUdpReply(msg, len, src_port);
}

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
  Serial.println("Initializing");

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
#if STATIC
  ether.staticSetup(myip, gwip);
#else
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
#endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  // register udpSerialPrint() to port 1337
  ether.udpServerListenOnPort(&udpSerialPrint, 1337);

  // register udpSerialPrint() to port 42.
  ether.udpServerListenOnPort(&udpSerialPrint, 42);
}

void loop()
{
  // this must be called for ethercard functions to work.
  ether.packetLoop(ether.packetReceive());
}
