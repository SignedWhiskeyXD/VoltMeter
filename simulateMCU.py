import datetime
import time
import serial
import random


def serialize_values(boost, uint16Val):
    # 转换为字节序列
    byte1 = bytes([boost])
    byte2 = bytes([(uint16Val >> 8) & 0xFF])
    byte3 = bytes([uint16Val & 0xFF])

    # 构建字节序列化字符串
    result = b"WSK" + byte1 + byte2 + byte3

    return result


portName = "COM15"
turns = 10000
simulate_val = 30000
PGAGain = 1
waitTime = 0.1
ser = serial.Serial(portName, baudrate=9600)

while turns:
    simulate_val += random.randrange(-1000, 1000)
    if simulate_val > 65535:
        simulate_val = 30000
    elif simulate_val < 0:
        simulate_val = 30000

    oops = False
    if random.randrange(0, 40) == 0:
        oops = True
        print("oops...",  datetime.datetime.now())

    if oops:
        ser.write(serialize_values(PGAGain, simulate_val + 10000))
    else:
        ser.write(serialize_values(PGAGain, simulate_val))
    time.sleep(waitTime)
    turns -= 1

ser.close()
