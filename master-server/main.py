# Importing flask module in the project is mandatory
# An object of Flask class is our WSGI application.
from flask import Flask, render_template, jsonify, request
import threading, time, socket, sys

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('127.0.0.1', 10000)

print('starting up on %s port %s' % server_address)
sock.bind(server_address)

# Flask constructor takes the name of
# current module (__name__) as argument.
app = Flask(__name__)

# The route() function of the Flask class is a decorator,
# which tells the application which URL should call
# the associated function.
cur_temp_from_udp = '120'
def check_incoming_traffic():
    while True:
        print('\nwaiting to receive message')
        data, address = sock.recvfrom(4096)

        print('received %s bytes from %s' % (len(data), address))
        print(data)
        if data:
            global cur_temp_from_udp
            cur_temp_from_udp = data
            sent = sock.sendto(data, address)
            print('sent %s bytes back to %s' % (sent, address))


@app.route('/')
def main():
    data = {'cur_temp': cur_temp_from_udp}
    return render_template("home.html", data = data)

@app.route('/_update_cur_temp')
def update_cur_temp():
    return jsonify(result = cur_temp_from_udp)

# main driver function
if __name__ == '__main__':
    t1 = threading.Thread(target=app.run)
    t2 = threading.Thread(target=check_incoming_traffic)

    t1.setDaemon(True)
    t2.setDaemon(True)
    t1.start()
    t2.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("exiting")
        exit(0)
