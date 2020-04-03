from flask import Flask, render_template, jsonify, request
from json import JSONEncoder
import threading, time, socket, sys, json
from DCPx_functions import *
from RTU_data import *

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
RTU_list = []
RTU_list.append(RTU_data(id=2, ip='192.168.2.177', port=8888, server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM), server_address = ('192.168.2.111', 10000)))
RTU_list.append(RTU_data(id=3, ip='192.168.1.178', port=8888, server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM), server_address = ('192.168.1.111', 10000)))
def get_RTU_i(id):
    for i in range(len(RTU_list)):
        if RTU_list[i].id == id:
            return i
    return -1

app = Flask(__name__)

def listening_thread(rtu):
    server_address = rtu.server_address
    print('starting up RTU id # %i on port %s' % (rtu.id, server_address))
    rtu.server_socket.bind(server_address)
    while True:
        data, address = rtu.server_socket.recvfrom(4096)
        data_list = bytearray(data)
        if len(data_list) > 3:
            if DCP_is_valid_response(data_list, rtu):
                DCP_process_response(data_list, RTU_list[get_RTU_i(data_list[2])])


@app.route('/')
def main():
    return render_template("home.html")

# responding to JSON request
@app.route('/_update_cur_temp/<rtu_id>')
def update_cur_temp(rtu_id):
    if get_RTU_i(int(rtu_id)) == -1:
        print("no rtu found with id of %s" % rtu_id)
        return
    buff = bytearray(DCP_buildPoll(int(rtu_id), DCP_op_lookup(DCP_op_name.FUDR)))
    DCP_compress_AA_byte(buff)
    sent = RTU_list[get_RTU_i(int(rtu_id))].server_socket.sendto(buff, (RTU_list[get_RTU_i(int(rtu_id))].ip, RTU_list[get_RTU_i(int(rtu_id))].port))
    print("sending data for RTU %s with array size of %i" %(rtu_id, len(RTU_list[get_RTU_i(int(rtu_id))].history)))
    return json.dumps(RTU_list[get_RTU_i(int(rtu_id))], indent=4, cls=RTU_data_Encoder)


if __name__ == '__main__':
    t1 = threading.Thread(target=app.run)
    t1.setDaemon(True)

    threads = []
    for rtu in RTU_list:
        threads.append(threading.Thread(target=listening_thread, args=(rtu,)))
        threads[len(threads)-1].setDaemon(True)

    t1.start()
    for rtu in threads:
        rtu.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("exiting")
        exit(0)
