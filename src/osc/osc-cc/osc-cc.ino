/*---------------------------------------------------------------------------------------------
  Send OSC messages for analog inputs, with hysteresis

  Send value changes only using a very basic method of hysteresis to avoid
  noise. This method uses less network bandwidth, and has better latency, since
  messages are transmitted when appropriate, rather than after waiting for a poll
  interval to elapse.

  Multiplexing example over serial:
  https://kevinsaye.wordpress.com/2018/01/07/adding-a-16-channel-multiplexor-to-your-esp8266-using-arduino/

  This example coerces the raw integer reading from the ADC to a float, here we
  keep the ADC reading as an integer and transmit that value as an integer over
  OSC.
  --------------------------------------------------------------------------------------------- */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

// Arduino IP
//IPAddress ip(999, 999, 99, 999); // put actual ip here
char ssid[] = "*********";         // your network SSID (name)
char pass[] = "********";          // your network password

const IPAddress outIp(999, 999, 99, 999); // remote IP of your computer
const unsigned int outPort = 9999;        // remote port to receive OSC
const unsigned int localPort = 8888;      // local port to listen for OSC

// wire (multiplexor)EN to (ESP)GND, SIG to A0, VCC to 3v3 and GND to GND
int MUXPinS0 = 16;
int MUXPinS1 = 0;
int MUXPinS2 = 15;
int MUXPinS3 = 13;

// Control names for OSC paths
// Pure data certainly gets confused about types when using integers
char controlNames[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'};

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
void send_osc_msg(char controlId, int currentValue) {
  // Serial debug
  Serial.print("sending OSC control ");
  Serial.print(controlId);
  Serial.print(": ");
  Serial.println(currentValue);

  // Create message object with control's path
  OSCMessage msg("/control/".concat(controlNames[controlId]));
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
    send_udp_packet(controlId, currentValue);
  }

  // update the current value
  currentValues[controlId] = currentValue;
}
