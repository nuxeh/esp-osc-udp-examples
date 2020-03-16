/*---------------------------------------------------------------------------------------------
  Send plain UDP frames for analog inputs, polling

  Send a UDP frame containing all values after a poll interval has elapsed.

  Each control has two bytes in the frame sent in order to transmit 10-bit
  values.

  Byte
  1 - high byte of controller value
  2 - low byte of controller value
  --------------------------------------------------------------------------------------------- */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

// WIFI
char ssid[] = "*********";         // your network SSID (name)
char pass[] = "********";          // your network password

// Network
const IPAddress outIp(999, 999, 99, 999); // remote IP of your computer
const unsigned int outPort = 9999;        // remote port to receive OSC
const unsigned int localPort = 8888;      // local port to listen for OSC

// wire (multiplexor)EN to (ESP)GND, SIG to A0, VCC to 3v3 and GND to GND
int MUXPinS0 = 16;
int MUXPinS1 = 0;
int MUXPinS2 = 15;
int MUXPinS3 = 13;

// Poll period (milliseconds)
int pollPeriod = 1000;

void setup() {
  // Start serial
  Serial.begin(115200);
  Serial.println("UDP frame");

  // Configure mux pins
  pinMode(MUXPinS0, OUTPUT);
  pinMode(MUXPinS1, OUTPUT);
  pinMode(MUXPinS2, OUTPUT);
  pinMode(MUXPinS3, OUTPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP
  Udp.begin(localPort);
}

void loop() {
  // Create a new bundle object
  char udpFrame[33] = {0};
  udpFrame[32] = '\0'; // Null termination character

  for (char i = 0; i < 16; i++) {
    int value = getAnalog(i); // Get the current analogue value of control i

    // Serial debug
    Serial.print(value);
    Serial.print(" ");

    // Add this control to the UDP frame
    udpFrame[2*i] = value >> 8;       // Most significant byte
    udpFrame[(2*i)+1] = value & 0xFF; // Least significant byte

    // Throttle
    delay(10);
  }

  // Serial debug
  Serial.println("");

  // Send the bundle over UDP
  Udp.beginPacket(outIp, outPort); // Start of UDP packet
  Udp.write(udpFrame);             // write UDP packet
  Udp.endPacket();                 // End of UDP packet 

  // Poll delay
  delay(pollPeriod);
}

// Get a 10-bit integer for the mux input
int getAnalog(int MUXyPin) {
  //MUXyPin must be 0 to 15 representing the analog pin you want to read
  //MUXPinS3 to MUXPinS0 are the Arduino pins connected to the selector pins of this board.
  digitalWrite(MUXPinS3, HIGH && (MUXyPin & B00001000));
  digitalWrite(MUXPinS2, HIGH && (MUXyPin & B00000100));
  digitalWrite(MUXPinS1, HIGH && (MUXyPin & B00000010));
  digitalWrite(MUXPinS0, HIGH && (MUXyPin & B00000001));

  // Converting to float is redundant, keep 10-bit integer
  return analogRead(A0);
}

