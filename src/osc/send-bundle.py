from pythonosc import udp_client
from pythonosc import osc_bundle_builder
from pythonosc import osc_message_builder
from random import randint
from time import sleep

ip = "127.0.0.1"
port = 5007
client = udp_client.SimpleUDPClient(ip, port)

channels = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"]

def make_message(ctrlName, value):
    msg = osc_message_builder.OscMessageBuilder(address="/control/" + ctrlName)
    msg.add_arg(value)
    return msg.build()

def send_bundle():
    bundle = osc_bundle_builder.OscBundleBuilder(osc_bundle_builder.IMMEDIATELY)
    for c in channels:
        bundle.add_content(make_message(c, randint(0, 1024)))
    sub_bundle = bundle.build()
    bundle.add_content(sub_bundle)
    bundle = bundle.build()

    client.send(bundle)

for i in range(0, 20):
    send_bundle()
    print("sent OSC bundle " + str(i))
    sleep(0.1)
