# Importing flask module in the project is mandatory
# An object of Flask class is our WSGI application.
from flask import Flask, render_template, jsonify, request
from json import JSONEncoder
import threading, time, socket, sys, json
from DCPx_functions import *
from RTU_data import *

# class RTU_data_Encoder(JSONEncoder):
#     def default(self, o):
#         return o.__dict__

class RTU_data_Encoder(JSONEncoder):
    def default(self, obj):
        if isinstance(obj, set):
            return list(obj)
        if isinstance(obj, Dttimetemphum):
            return [obj.year, obj.month, obj.day, obj.hour, obj.minute, obj.second, obj.temp, obj.hum]
        if isinstance(obj, RTU_data):
            return [obj.id, obj.thresholds, obj.alarms_binary, obj.current_data, list(obj.history)]
        return json.JSONEncoder.default(self, obj)

# CONSTANTS
RTU_id = 2
RTU2_id = 3
RTU_list = [RTU_data(id=RTU_id, ip='192.168.2.177', port=8888), RTU_data(id=RTU2_id, ip='192.168.1.178', port=8888)]
def get_RTU_i(id):
    for i in range(len(RTU_list)):
        if RTU_list[i].id == id:
            return i
    return -1

# Flask constructor takes the name of
# current module (__name__) as argument.
app = Flask(__name__)

# The route() function of the Flask class is a decorator,
# which tells the application which URL should call
# the associated function.
cur_temp_from_udp = 120
cur_hum_from_udp = 1000
# function to continuously check for UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('192.168.1.111', 10000)
print('starting up on %s port %s' % server_address)
sock.bind(server_address)
def check_incoming_traffic():
    while True:
        # print('\nwaiting to receive message')
        data, address = sock.recvfrom(4096)
        data_list = bytearray(data)
        # print('received %s bytes from %s' % (len(data), address))
        # print(':'.join(x.encode('hex') for x in data))
        if data:
            if DCP_is_valid_response(data_list):
                if (get_RTU_i(data_list[2]) != -1):
                    RTU_address = (RTU_list[get_RTU_i(data_list[2])].ip, RTU_list[get_RTU_i(data_list[2])].port)
                    DCP_process_response(data_list, RTU_list[get_RTU_i(data_list[2])])
                    #print(RTU_list[get_RTU_i(data_list[2])])
                # global cur_temp_from_udp
                # cur_temp_from_udp = data
                # sent = sock.sendto(data, address)
                # sent = sock.sendto(bytearray(DCP_buildPoll(RTU_id, DCP_op_lookup(DCP_op_name.FUDR))), address)
                # print('sent %s bytes back to %s' % (sent, address))


@app.route('/')
def main():
    return render_template("home.html")

# responding to JSON request
@app.route('/_update_cur_temp/<rtu_id>')
def update_cur_temp(rtu_id):
    # sending a request to RTU
    if get_RTU_i(int(rtu_id)) == -1:
        print("no rtu found with id of %s" % rtu_id)
        return
    buff = bytearray(DCP_buildPoll(int(rtu_id), DCP_op_lookup(DCP_op_name.FUDR)))
    DCP_compress_AA_byte(buff)
    sent = sock.sendto(buff, (RTU_list[get_RTU_i(int(rtu_id))].ip, RTU_list[get_RTU_i(int(rtu_id))].port))
    print(' '.join(hex(i) for i in bytearray(DCP_buildPoll(int(rtu_id), DCP_op_lookup(DCP_op_name.FUDR))))),
    print('sent to RTU at %s:%s' % (RTU_list[get_RTU_i(int(rtu_id))].ip, RTU_list[get_RTU_i(int(rtu_id))].port))

    """
    print("debug station")
    testlist = [0xaa, 0xfc, 0xaa, 0x12, 0xaa, 0xaa, 0x64, 0xaa, 0xaa, 0xaa, 0xaa, 0xab, 0xaa]
    testlist2 = [0xaa, 0xfc, 0x12, 0x64, 0xab]
    print(':'.join(hex(i)[2:] for i in testlist))
    DCP_compress_AA_byte(testlist)
    print(':'.join(hex(i)[2:] for i in testlist))
    DCP_expand_AA_byte(testlist)
    print(':'.join(hex(i)[2:] for i in testlist))


    print(':'.join(hex(i)[2:] for i in testlist2))
    DCP_compress_AA_byte(testlist2)
    print(':'.join(hex(i)[2:] for i in testlist2))
    DCP_expand_AA_byte(testlist2)
    print(':'.join(hex(i)[2:] for i in testlist2))
    """
    # return jsonify(cur_temp = cur_temp_from_udp, cur_hum = cur_hum_from_udp, whole_data = RTU_list[get_RTU_i(int(rtu_id))])
    return json.dumps(RTU_list[get_RTU_i(int(rtu_id))], indent=4, cls=RTU_data_Encoder)
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
