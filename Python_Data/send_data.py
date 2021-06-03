# Importing Libraries
import serial
import time

import sys

ado = serial.Serial(port='COM3', baudrate=115200, timeout=0.2)
time.sleep(1)

def write_read(x):
    s = str(x) + ',' + str(x+1) + ',' + str(x+2)
    print("t", s)
#    s = bytes([num]) + bytes([num+1]) + bytes([num+2])
    ado.write(bytes(s,  'utf-8'))
    time.sleep(0.5)
    value = ado.readline()
    
    return value

num = 0

def ac(ado = ado):
    ado.close()

while True:
    num = num + 10
    value = write_read(num)
    print(value)
#    print(value) # printing the value
#    if len(value) == 4 and value[3] == 10:
#        [x, y, z] = value[0:3]
#        print("r", x, y, z)
   