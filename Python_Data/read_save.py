
import numpy as np
# import matplotlib.pyplot as plt

# from datetime import date
# today = date.today()

# dire = ''
# setfile = str(today) + "_setting.txt"
# fs = open(setfile, 'w')
# fs.write(str(today))
# fs.write("threshold = 200 mV \n")
# fs.write("Current = 1.8A \n")
# fs.write("f = 10kHz \n")

filename ='test.csv' # save file name
 
import serial   
f = open(filename,'w')
print(filename)
print('Serial Open')

err = 0
with serial.Serial("com5", 115200) as ser:
    t = 0 # how many samples?
    for line in ser:
        # print("lineLength = ", len(line))

        if (len(line) >= 29 and len(line) <= 34):
            t = t+1
            try:
                a = line.decode("ascii")
            except: continue                        
            f.write(a)
            if (t%100 == 1):
                print('t=', t)
            if t > 20000: # collect t of samples 
                break
        
        elif (len(line)> 35):
            try:
                a = line.decode("ascii")
                print(a)
            except: continue   
            
ser.close()
f.close()

#%%
dst = np.genfromtxt(filename, delimiter=',')

dst = dst[100:]
dst

tof = dst[:, 2:4]

print("Tof1        Tof2")
print(np.mean(tof, axis = 0)) 
print(np.std(tof, axis = 0)) # For calibration, check if std  < 20 
