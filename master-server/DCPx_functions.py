
from RTU_data import *

class DCP_op_name:
    FUDR = 1


class DCP_op_entry:
    text = ""
    code = 0
    len = 0
    def __init__(self, text = '', code = 0, len = 0):
        self.text = text
        self.code = code
        self.len = len

DCP_op_table = []
DCP_op_table.append(DCP_op_entry('FUDR', DCP_op_name.FUDR, 2))

def to_int8_t(x):
    return (x if x < 128 else x - 256)

def DCP_op_lookup(op):
    for x in DCP_op_table:
        if (x.code == op):
            return x

def DCP_genCmndBCH(buffer, count):
    bch, nBCHpoly, fBCHpoly = (0, 0xb8, 0xff)
    # 2 for accounting framing bytes
    for i in range(2, count):
        bch ^= buffer[i]
        for j in range(8):
            if ((bch & 1) == 1):
                bch = (bch >> 1) ^ nBCHpoly
            else:
                bch >>= 1
    bch ^= fBCHpoly
    return bch

def DCP_compress_AA_byte(buffer):
    i = 2
    while (i < len(buffer) - 1):
        aa_counter = 1
        # if the next one is also AA
        if (buffer[i] == 0xaa and buffer[i+1] == 0xaa):
            buffer.pop(i+1)
            aa_counter+=1
            j = i + 1
            # check how many AA bytes, and insert when done
            while (j < len(buffer)):
                if (buffer[j] == 0xaa):
                    buffer.pop(j)
                    aa_counter+=1
                else:
                    buffer.insert(j, aa_counter)
                    break
        # if AA byte but not repeating add a counter of how many
        elif (buffer[i] == 0xaa):
            buffer.insert(i+1, 1)
        i+=1
    # take care of the last AA if exists
    if buffer[len(buffer)-1] == 0xaa:
        buffer.append(1)

def DCP_expand_AA_byte(buffer):
    # take care of the last AA if exists
    if (buffer[len(buffer)-2] == 0xaa and buffer[len(buffer)-1] == 1):
        buffer.pop(len(buffer)-1)
    i = 2
    while (i < len(buffer) - 1):
        # if AA is fond
        if (buffer[i] == 0xaa and buffer[i-1] != 0xaa and buffer[i+1] != 0xaa):
            aa_counter = buffer.pop(i+1)
            aa_counter -= 1 #takes care is only 1 AA no need to insert
            # reinsert required bytes
            while aa_counter:
                buffer.insert(i+1, 0xaa)
                aa_counter -= 1
        i+=1


def DCP_buildPoll(address, command):
    buff = []
    # command frame setup
    buff.append(0xaa)
    buff.append(0xfc)
    buff.append(address)
    buff.append(command.code)
    buff.append(DCP_genCmndBCH(buff, len(buff)))
    return buff

def DCP_is_valid_response(buffer, rtu):
    return (buffer[len(buffer)-1] == DCP_genCmndBCH(buffer, len(buffer)-1) and buffer[0] == 0xaa and buffer[1] == 0xfa and buffer[2] == rtu.id)

def DCP_process_response(buffer, rtu_data):
    """
    response packet to this is going to look like this
    [aa][fa][addr][opcode][00][val1][val2][val3][val4][alarms][bch] - means sending the threshold in C
    [aa][fa][addr][opcode][01][dt1][dt2][dt3][dt4][dt5][dt6][temp][hum][bch] - current temp
    [aa][fa][addr][opcode][02][dt1][dt2][dt3][dt4][dt5][dt6][temp][hum][bch] - year month sent in bitwise with temp and humidity
    """
    if buffer[3] == DCP_op_name.FUDR: #get the command to process
        if buffer[4] == 0:#updating the thresholds
            rtu_data.set_thresholds([to_int8_t(buffer[5]), to_int8_t(buffer[6]), to_int8_t(buffer[7]), to_int8_t(buffer[8])])
            rtu_data.set_alarms_binary(buffer[9])
        if buffer[4] == 1:#updating the current
            rtu_data.set_current_data(Dttimetemphum(buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], to_int8_t(buffer[11]), buffer[12]))
        if (buffer[4] == 2):#updating the history
            temp_data = Dttimetemphum(buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], to_int8_t(buffer[11]), buffer[12])
            rtu_data.add_hist(temp_data)
