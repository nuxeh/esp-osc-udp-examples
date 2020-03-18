/*---------------------------------------------------------------------------------------------
  Send OSC bundles for analog inputs, polling

  Send an OSC bundle containing all values after a poll interval has elapsed.

  The reading from analogue controls is sent over OSC as a 32-bit integer
  message parameter, in messages packed into a single OSC bundle.

  Control paths are, for example:
  - /control/a
  - /control/b
  - /control/c
  - /control/d
  etc.
  --------------------------------------------------------------------------------------------- */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

// WIFI
char ssid[] = "*********"; // your network SSID (name)
char pass[] = "********";  // your network password

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
  Serial.println("OSC bundle");

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
  OSCBundle bundle;

  // Get all analogue values in turn
  for (char i = 0; i < 16; i++) {
    int value = getAnalog(i); // Get the current analogue value of control i

    // Serial debug
    Serial.print(value);
    Serial.print(" ");

    // Add a message for this control to the bundle, with 32-bit int parameter
    switch (i) {
      case 0:
        bundle.add("/control/a").add((int32_t)value);
        break;
      case 1:
        bundle.add("/control/b").add((int32_t)value);
        break;
      case 2:
        bundle.add("/control/c").add((int32_t)value);
        break;
      case 3:
        bundle.add("/control/d").add((int32_t)value);
        break;
      case 4:
        bundle.add("/control/e").add((int32_t)value);
        break;
      case 5:
        bundle.add("/control/f").add((int32_t)value);
        break;
      case 6:
        bundle.add("/control/g").add((int32_t)value);
        break;
      case 7:
        bundle.add("/control/h").add((int32_t)value);
        break;
      case 8:
        bundle.add("/control/i").add((int32_t)value);
        break;
      case 9:
        bundle.add("/control/j").add((int32_t)value);
        break;
      case 10:
        bundle.add("/control/k").add((int32_t)value);
        break;
      case 11:
        bundle.add("/control/l").add((int32_t)value);
        break;
      case 12:
        bundle.add("/control/m").add((int32_t)value);
        break;
      case 13:
        bundle.add("/control/n").add((int32_t)value);
        break;
      case 14:
        bundle.add("/control/o").add((int32_t)value);
        break;
      case 15:
        bundle.add("/control/p").add((int32_t)value);
        break;
    }

    // Throttle
    delay(10);
  }

  // Serial debug
  Serial.println("");

  // Send the bundle over UDP
  Udp.beginPacket(outIp, outPort); // Start of UDP packet
  bundle.send(Udp);                // send the OSC frame over UDP
  Udp.endPacket();                 // End of UDP packet 
  bundle.empty();                  // may not be necessary

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
