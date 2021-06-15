
import numpy as np

from time import strftime
 
time = strftime("%d_%H_%M")

filename = 'imu_' + time + '_C.csv'
 
import serial   
f = open(filename,'w')
print(filename)
print('Serial Open')

err = 0
with serial.Serial("com15", 115200) as ser:
    t = 0 # how many samples?
    for line in ser:
        # print("lineLength = ", len(line))

        if (len(line) >= 26 and len(line) <= 35):
            t = t+1
            try:
                a = line.decode("ascii")
            except: continue                        
            f.write(a)
            if (t%100 == 1):
                print('t=', t)
            if t > 40000: # collect t of samples 
                break
        
        elif (len(line)> 35):
            try:
                a = line.decode("ascii")
                print(a)
            except: continue   
            
ser.close()
f.close()

#%%

import matplotlib.pyplot as plt

dst = np.genfromtxt(filename, delimiter=',')

dst = dst[1:]

imu = dst[:, 4:6]
plt.figure(1)
plt.plot(imu)
plt.xlabel('Samples')
plt.ylabel('IMU')
plt.title('raw Data')


# tof = dst[:, 2:4]

# print("Tof1        Tof2")
# print(np.mean(tof, axis = 0)) 
# # print(np.std(tof, axis = 0)) # For calibration, check if std  < 20 
# plt.figure(2)
# plt.plot(tof)
# plt.xlabel('Samples')
# plt.ylabel('tof(ns)')
# plt.title('raw Data')


