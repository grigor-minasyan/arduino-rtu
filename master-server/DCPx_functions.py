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
