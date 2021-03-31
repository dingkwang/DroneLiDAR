
import numpy as np

from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import serial
import joblib

import time

modelname = 'model_i.sav'

fn = 'test.csv' # Save data
file = open(fn, 'w')

model = joblib.load(modelname)

def Sergetline():
    while (1):
        line= ser.readline()
        # print("lineLength = ", len(line))
        if len(line) >= 29 and len(line) <= 33 : # simple error check
            # print(line)
            try:
                 a = line.decode("ascii")
            except: continue
            # print("a=", a)
            file.write(a)
            
            lst = a.split(',')
            dat = [float(i) for i in lst]
            if len(dat) == 8:
                return dat
        # else: 
             # line= ser.readline()

def pol2cart(thx, thy, d, thre_f, thre_c):
    for i in range(len(d)):
        if d[i] > thre_f:
            d[i] = thre_f + np.random.rand()/2
        elif d[i] < thre_c:
            d[i] = thre_c + np.random.rand()/2

    thxd, thyd = np.radians(thx), np.radians(thy)
    dz = np.sqrt(1/(np.cos(thxd))**2+1/(np.cos(thyd))**2-1)
    z = d/dz
    x = z / np.cos(thy) * np.tan(thx)
    x = np.sqrt(abs(d**2- (z/np.cos(thyd))**2)) * np.sign(thx)
    y = np.sqrt(abs(d**2- (z/np.cos(thxd))**2)) * np.sign(thy)
    
    # z = z-20 + random.random()
    # z = z
    zm = thre_f-z # flip the point cloud 
    npt = np.transpose(np.array([x, y, zm]))
    colorz = (z - thre_c+2)/(thre_f-thre_c+2)
    return npt, colorz


sizex = 16
sizey = 16
sizeall = sizex * sizey

frame_dat = np.zeros((sizeall, 8), dtype="float") ## store raw data[Scanx, scanY, tof1, tof2]

pos3 = np.empty((sizex*sizey,3))

color = np.empty((len(pos3),4), dtype=np.float32)

color[:,0] = 1
color[:,1] = 1
color[:,2] = 1
color[:,3] = 1

thre_f = 300
thre_c = 200

#Collect whole frame data 
xyi = 0
xi, yi = -1, -1

print("Collecting first frames...")
i = 0

with serial.Serial("com5", 115200) as ser:  # check arduino port
    
    while(i<sizeall+10):
        line_dat = Sergetline()
        xi = int(line_dat [6])
        yi = int(line_dat[7])
  
        xi = xi % sizex
        yi = yi % sizey
    
        # print("xi=", xi, "yi=", yi)
        xyi = int(xi + yi * sizex)
    
        frame_dat[xyi, :] = line_dat
        i = i + 1


print("First Frame Collected ")

pred = model.predict(frame_dat[:, :4])
new_pt, colorz = pol2cart(frame_dat[:, 0], frame_dat[:, 1], pred, thre_f, thre_c)
pos3 = new_pt
color[:, 0] = colorz
color[:, 1] = 1-colorz 


app = QtGui.QApplication([])
w = gl.GLViewWidget()
w.show()
w.setWindowTitle('pyqtgraph example: GLScatterPlotItem')
w.setFixedSize(500, 600)
w.setCameraPosition(distance=160, elevation = 90, azimuth=270)


g = gl.GLGridItem()
w.addItem(g)

sp3 = gl.GLScatterPlotItem(pos=pos3, color=color, size=1.5, pxMode=False)

sp3.setData(pos=pos3, color=color)
w.addItem(sp3)


#%%
t = 0
datx = np.zeros((sizex,len(line_dat)))
ave = 0.85
def update():
    global sp3, pos3, npt, model, t, color, sizeall, sizex, thre_f, thre_c, frame_dat, datx
    global ave
    i = 0
    with serial.Serial("com5", 115200) as ser:  
        while(i<(sizex-1)):
            
            line= ser.readline()
            
            line_dat = Sergetline(line)
            
            datx[i] = line_dat
            i = i + 1


    ###
    # datx[np.isnan(datx).any(axis=1), :] = 0 # remove nan
    # datx[np.less(datx[2:3], 100).any(axis=1), :] = np.nan # remove
    
    imu = datx[5, 4:6]
    
    # print("tem_dat.head", tem_dat[0, :])
    print("imu x, y: {:+5.1f}, {:+5.1f}".format(imu[0], imu[1]))
    pred = model.predict(datx[:, :4])
    
    new_pt, colorz = pol2cart(datx[:, 0], datx[:, 1], pred, thre_f, thre_c)
    
    new_pt[np.less(new_pt[:, 2:], 0.1).any(axis=1), :] = 1000
    # pos3 = npt*ave + pos3*(1-ave)
    # color[:, 0] = colorz*ave + color[:, 0] * (1-ave)
    # color[:, 1] = (1-colorz)*ave + color[:, 1]*(1-ave)
    
    xi = datx[:, 6]
    yi = datx[:, 7] 
   
    # update 
    xi = xi % sizex
    yi = yi % sizey
    xyi = (xi + yi * sizex).astype(int)
    
    # xi = datx[:, 10]%sizex
    # yi = datx[:, 11] 
    # xyi = (xi + yi * sizex).astype(int)
    # pos3[xyi, :] = npt
    # print("xi", xi)
    # print("npt ", new_pt[:, 2])
    # print("pos3", pos3[xyi, 2])
    time.sleep(0.02)
    # time.sleep(1)
    pos3[xyi, 2] = new_pt[:, 2]*ave + pos3[xyi, 2]*(1-ave)
    
    color[xyi, 0] = colorz*ave + color[xyi, 0] * (1-ave)
    color[xyi, 1] = (1-colorz)*ave + color[xyi, 1]*(1-ave)

    sp3.setData(pos=pos3,color = color)
    app.processEvents()
    
timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(0)

if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

