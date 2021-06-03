# Importing Libraries
import serial
import time

import ctypes

import numpy as np

import sys



def calculate_imu_crc(data, length):
    i = 0
    crc = np.ushort(0) # unsigned short 
    for i in range(length):
        crc  = np.ushort(np.ushort(crc >> 8) | np.ushort(crc << 8))
#        crc = np.ushort(crc ^ data[i])
        crc  ^= data[i]
        crc = np.ushort(crc)
        crc = crc ^ np.ushort(np.ushort(crc & 0xff) >> 4)
        crc = np.ushort(crc)
        
        crc ^= np.ushort(np.ushort(crc << 12))
        crc = np.ushort(crc)
        
        crc ^= np.ushort(np.ushort(crc & 0x00ff) << 5)
        crc = np.ushort(crc)
    return crc
  
  
  

length = 4
num = 11


data = num.to_bytes(length, byteorder='little')

crc1 = calculate_imu_crc(data, length)

crc1 = np.ushort(crc1)
crcb = crc1.tobytes()
data = data + crcb


ado = serial.Serial(port='COM3', baudrate=115200, timeout=0.2, write_timeout = 1)
time.sleep(1)


def ac(ado = ado):
    ado.close()
#%%

#
def write_read(num):
    data = num.to_bytes(length, byteorder='little')
    crc1 = calculate_imu_crc(data, length)
    crcb = crc1.tobytes()
    data = data + crcb
    # print('data: ', data)
    # ss = str(num) + ',' + str(num+1) + ',' + str(num+2)
    # ado.write(ss)
    try:
        ado.write(data)
    except:
        pass
    
#    ado.flush()
    time.sleep(0.1)
    value = ado.readline()
#    value = ado.read(4)
    # print(value)    
    return value

#
while True:
    value = write_read(num)
    print(value)
#    print(value) # printing the value
#    if len(value) == 4 and value[3] == 10:
#        [x, y, z] = value[0:3]
#        print("r", x, y, z)
   