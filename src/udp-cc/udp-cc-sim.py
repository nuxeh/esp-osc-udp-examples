import socket
import time

UDP_IP = "localhost"
UDP_PORT = 5005

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_cc(cntId, val):
    packet = [0, 0, 0]

    packet[0] = cntId
    packet[1] = val >> 8
    packet[2] = val & 0xFF

    string = chr(packet[0]) + chr(packet[1]) + chr(packet[2])

    print("sending: " + str(packet) + " [" + str(cntId) + "," + str(val) + "]")
    sock.sendto(string, (UDP_IP, UDP_PORT))

send_cc(2, 1024)
send_cc(2, 0)
send_cc(2, 0xFF)
send_cc(65, 65)

def do_show():
    for i in range(0, 1024):
        send_cc(0, i)
        send_cc(1, 1024-i)
        send_cc(2, i)
        send_cc(3, 1024-i)
        send_cc(4, i)
        send_cc(5, 1024-i)
        send_cc(6, i)
        send_cc(7, 1024-i)
        send_cc(8, i)
        send_cc(9, 1024-i)
        send_cc(10, i)
        send_cc(11, 1024-i)
        send_cc(12, i)
        send_cc(13, 1024-i)
        send_cc(14, i)
        send_cc(15, 1024-i)
        time.sleep(0.1)

do_show()
