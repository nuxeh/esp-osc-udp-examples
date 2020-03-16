
from pythonosc import udp_client
from pythonosc import osc_message_builder
from random import randint
from time import sleep

ip = "127.0.0.1"
port = 5007
client = udp_client.SimpleUDPClient(ip, port)

channels = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"]
j = 0

def make_message(ctrlName, value):
    msg = osc_message_builder.OscMessageBuilder(address="/control/" + ctrlName)
    msg.add_arg(value)
    return msg.build()

def send_messages():
    global j
    for c in channels:
        print("sent OSC message " + str(j))
        j += 1
        client.send(make_message(c, randint(0, 1024)))

for i in range(0, 20):
    send_messages()
    sleep(0.1)
