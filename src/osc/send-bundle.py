from pythonosc import udp_client
from pythonosc import osc_bundle_builder
from pythonosc import osc_message_builder

ip = "127.0.0.1"
port = 5007
client = udp_client.SimpleUDPClient(addr, port)

def make_message(ctrlName, value):
    msg = osc_message_builder.OscMessageBuilder(address="/control/" + ctrlName)
    msg.add_arg(value)
    return msg.build()

def send_bundle():
    bundle = osc_bundle_builder.OscBundleBuilder(osc_bundle_builder.IMMEDIATELY)
    bundle.add_content(make_message("a", 512))
    bundle.add_content(make_message("c", 512))
    bundle.add_content(make_message("e", 512))
    bundle.add_content(make_message("n", 512))
    bundle.add_content(make_message("o", 512))
    bundle.add_content(make_message("p", 512))
    sub_bundle = bundle.build()
    bundle.add_content(sub_bundle)
    bundle = bundle.build()

    client.send(bundle)

send_bundle()
