
import numpy as np

filename = 'c300.csv' # save file name
 
import serial   
f = open(filename,'w')
print(filename)
print('Serial Open')

err = 0
with serial.Serial("com15", 115200) as ser:
    t = 0 # how many samples?
    for line in ser:
        # print("lineLength = ", len(line))

        if (len(line) >= 35 and len(line) <= 41):
            t = t+1
            try:
                a = line.decode("ascii")
            except: continue                        
            f.write(a)
            if (t%100 == 1):
                print('t=', t)
            if t > 2000: # collect t of samples 
                break
        
        elif (len(line)> 44):
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
