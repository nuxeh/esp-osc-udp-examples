/*---------------------------------------------------------------------------------------------
  Send OSC messages for analog inputs, with hysteresis

  Send value changes only using a very basic method of hysteresis to avoid
  analogue noise. This method uses less network bandwidth, and has better
  latency, since messages are transmitted when appropriate, rather than after
  waiting for a poll interval to elapse.

  The reading from analogue controls is sent over OSC as a 32-bit integer
  message parameter.

  Control paths are, for example:
  - /control/a
  - /control/b
  - /control/c
  - /control/d
  etc.
  --------------------------------------------------------------------------------------------- */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

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

// Dead band threshold for simple hysteresis
// The analogue value needs to change more than this value to trigger sending a
// message for that control. This eliminates unnecessary sending of messages
// due to thermal/analogue noise. You can play with this value to achieve
// higher sensitivity (low) or higher rejection of noise (high). The scale is
// 0-1024. The threshold is +/-, each side of the last value, so the width of
// the band is 2*deadZoneThreshold
int deadZoneThreshold = 10;

// Current values of the analogue inputs
int currentValues[16] = {0};

void setup() {
  // Start serial
  Serial.begin(115200);
  Serial.println("OSC control change");

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
  for (char i = 0; i < 16; i++) {
    int value = getAnalog(i); // get the current value of control i

    // check if the control value has changed enough, and send a packet if
    // necessary.
    check_and_send_osc_msg(i, value);

    // Throttle
    delay(10);
  }
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

// Construct and send a UDP packet for one control
void send_osc_message(char controlId, int currentValue) {
  // Serial debug
  Serial.print("sending OSC control ");
  Serial.print(controlId);
  Serial.print(": ");
  Serial.println(currentValue);

  // Create message
  OSCMessage msg;

  // Create message object with control's path
  switch (controlId) {
    case 0:
      msg = OSCMessage("/control/a");
      break;
    case 1:
      msg = OSCMessage("/control/b");
      break;
    case 2:
      msg = OSCMessage("/control/c");
      break;
    case 3:
      msg = OSCMessage("/control/d");
      break;
    case 4:
      msg = OSCMessage("/control/e");
      break;
    case 5:
      msg = OSCMessage("/control/f");
      break;
    case 6:
      msg = OSCMessage("/control/g");
      break;
    case 7:
      msg = OSCMessage("/control/h");
      break;
    case 8:
      msg = OSCMessage("/control/i");
      break;
    case 9:
      msg = OSCMessage("/control/j");
      break;
    case 10:
      msg = OSCMessage("/control/k");
      break;
    case 11:
      msg = OSCMessage("/control/l");
      break;
    case 12:
      msg = OSCMessage("/control/m");
      break;
    case 13:
      msg = OSCMessage("/control/n");
      break;
    case 14:
      msg = OSCMessage("/control/o");
      break;
    case 15:
      msg = OSCMessage("/control/p");
      break;
  }

  // Add parameter
  msg.add((int32_t)currentValue);

  // Send UDP frame
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

// Check if the current value is over the threshold, and if it is, send a packet.
void check_and_send_osc_msg(char controlId, int currentValue) {
  int prevValue = currentValues[controlId];

  // check if the current value is outside the deadzone
  if (abs(prevValue - currentValue) > deadZoneThreshold) {
    send_osc_message(controlId, currentValue);
  }

  // update the current value
  currentValues[controlId] = currentValue;
}
