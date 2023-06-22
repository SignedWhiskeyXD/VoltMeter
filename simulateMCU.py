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
turns = 200
simulate_val = 30000

while turns > 0:
    ser = serial.Serial(portName, baudrate=9600)
    simulate_val += random.randrange(-1000, 1000)
    ser.write(serialize_values(1, simulate_val))
    ser.close()
    time.sleep(0.1)
    turns -= 1
