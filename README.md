# ESP8266 UDP/OSC examples

This repo contains the following:

- src/osc - OSC based protocol (polling and CC/hysteresis)
- src/udp-cc - plain UDP based CC messages with very simple hysteresis
- src/udp-frame - plain UDP frame containing all controller values (polling)

Each setup contains:

- A python script to simulate the sending of messages programatically
- An Arduino source file for ESP8266 for sending messages and reading analogue inputs
- A Pure data patch demonstrating receiving and routing the messages
- Image of the pd patch

The plain UDP setups represent very basic UDP based protocols, but from
experimentation, the OSC protocol is easier on the receiving side to implement
the necessary routing.
