# Importing Libraries
import serial
import time

ado = serial.Serial(port='COM3', baudrate=9600, timeout=.5)
def write_read(x):
    ss = str(num) + ',' + str(num+1) + ',' + str(num+2)
#    print(s)
    A = ado.write(bytes("1",  'utf-8'))
#    print(A)
    time.sleep(0.1)
    value = ado.readline()
    return value

num = 0

def ac():
    ado.close()

while True:
    num = num + 10
    value = write_read(num)
    print(value) # printing the value
   