/*

A simple serial to ethernet relay for the Hydronalix Teensy/Ethernet board.

Receives transmissions over Serial1 and spits them out via the provided 
ethernet shield. For debugging and viewing ethernet output, you'll need
Wireshark (https://www.wireshark.org/download.html) and a computer with an
ethernet port, or to be clever enough to utilize some other tool.

This was tested with the following setup:
- Mac OS Darwin with Ethernet network settings configured to 192.168.1.180
  running Wireshark
- Hydronalix Teensy Ethernet board running this program
- Auxilary Teensy transmitting "hello" over Serial1 every 100ms
- 5V power supply, breadboard

 */

#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <ArduinoJson.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 8888;              // "any port in a storm"
EthernetUDP Udp;                            // initialize your ethernet protocol (here I'm using UDP)

/**
 * sendStr is used as a buffer for receiving messages. for real applications, you'll probably
 * need to dynamically allocate the amount of memory, or use a bigger buffer for longer messages.
 */
char sendStr[6];

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

/**
 * function called in main loop that both reads the Serial1 line and transmits
 * UDP packets via Ethernet.
 */
void SerialUDPforward() {
  // create a JSON object to store data in, mostly for easy viewing in Wireshark
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(200));

  // first check if the Serial1 line is available
  if (Serial1.available()) {
    int i = 0;
    // while there's data, read it into a buffer. note: pre-defined buffer size isn't always the case!
    while (Serial1.available() && i < 6) {
      sendStr[i] = Serial1.read();
      i++;
    }

    // place the buffer data into the JSON object to be serialized and sent over UDP
    doc["data"] = sendStr; 
    // note the pre-defined IP and port: while port doesn't really matter, you may need
    //  to specify target IP for debugging/testing. If you're receiving UDP messages, you
    //  can use remoteIP() and remotePort() that grab the specified info from the last
    //  received UDP packet
    Udp.beginPacket(IPAddress(192, 168, 1, 180), 5005);
    serializeJson(doc, Udp);
    Udp.println();
    Udp.endPacket();
  } 
  
  // if there's no serial information, just send something over UDP. This is useful
  //  for debugging as well as keeping the ethernet connection alive!
  else {
    doc["data"] = "no updates on the serial line!"; 
    Udp.beginPacket(IPAddress(192, 168, 1, 180), 5005);
    serializeJson(doc, Udp);
    Udp.println();
    Udp.endPacket();
  }
}

/**
 * setup that runs once when device is powered on. Be sure that the baud rates
 * for the serial lines agree between communicating devices!
 */ 
void setup() {
  Ethernet.begin(mac, ip);
  delay(100);

  Udp.begin(localPort);
  delay(100);

  Serial.begin(115200);
  delay(100);

  Serial1.begin(9600);
}

/**
 * main program loop that runs infinitely on the device.
 */
void loop() {
  SerialUDPforward();
}



