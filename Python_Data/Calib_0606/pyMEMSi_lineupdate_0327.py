
import numpy as np

from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import random
import serial
import joblib

modelname = 'model_i.sav'
model = joblib.load(modelname)

def pol2cart(thx, thy, d, thre_f, thre_c):
    # thx= -(thx - 125)/255*6
    # thy = -(thy- 125)/255*6

    d = d
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
    zm = thre_f-z
    npt = np.transpose(np.array([x, y, zm]))
    colorz = (z - thre_c+2)/(thre_f-thre_c+2)
    return npt, colorz


sizex = 20
sizey = 20
sizeall = sizex * sizey


sercom = "com15"
frame_dat = np.empty((sizeall, 10), dtype="float") ## store raw data[Scanx, scanY, tof1, tof2]

pos3 = np.empty((sizex*sizey,3))


color = np.empty((len(pos3),4), dtype=np.float32)

color[:,0] = 1
color[:,1] = 1
color[:,2] = 1
color[:,3] = 1

thre_f = 300
thre_c = 200

ser=serial.Serial("COM15",115200)
# try:
#     ser.open()
#     print("SerialOpen")
# except:
#     print("e")
# ser.close()

# if not ser.isOpen():
#     ser.open()

#Collect whole frame data 
xyi = 0
print("Collecting irst frames...")

with serial.Serial("com5", 115200) as ser:
    
    while(xyi<sizeall-1):
        
        
        line = ser.readline()
    
        if (len(line) >= 55 and len(line) <= 66):
                try:
                    a = line.decode("ascii").strip()
                    rddat = a.split(',')
                    dat = np.array(rddat[0:12], dtype = 'float') #[angle, tof, imu, I, J]
                except:
                    print("E")
                    continue
        else:
            continue
        
    
        xi = dat [10] % sizex 
        yi = dat [11] 
        xyi = int(xi + yi * sizex)
        
        frame_dat[xyi, :] = dat[0:10]


frame_dat[np.isnan(frame_dat).any(axis=1), :] = 0 # remove nan
frame_dat[np.greater(abs(frame_dat), 1000000).any(axis=1), :] = 0 # remove nan

print("First Frame Collected ")


pred = model.predict(frame_dat)
npt, colorz = pol2cart(frame_dat[:, 0], frame_dat[:, 1], pred, thre_f, thre_c)
pos3 = npt
color[:, 0] = colorz
color[:, 1] = 1-colorz 

# ser.close()
#%%

app = QtGui.QApplication([])
w = gl.GLViewWidget()
w.opts['distance'] = 20
w.show()
w.setWindowTitle('pyqtgraph example: GLScatterPlotItem')
w.setFixedSize(500, 600)
w.setCameraPosition(distance=8, elevation = 90, azimuth=270)


g = gl.GLGridItem()
w.addItem(g)

sp3 = gl.GLScatterPlotItem(pos=pos3, color=color, size=0.1, pxMode=False)

sp3.setData(pos=pos3, color=color)
w.addItem(sp3)


# if __name__ == '__main__':
#     import sys
#     if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
#         QtGui.QApplication.instance().exec_()
        
#%%
t = 0
datx = np.zeros((sizex,len(dat)))
ave = 0.85
def update():
    global sp3, pos3, npt, model, t, color, sizeall, sizex, thre_f, thre_c, frame_dat, datx
    global ave
    i = 0
    while(i<(sizex-1)):
        line = ser.readline()
        if (len(line) >= 54 and len(line) <= 66):
                try:
                    a = line.decode("ascii").strip()
                    rddat = a.split(',')
                    imu = rddat[8:10]
                    datx[i] = np.array(rddat[0:12], dtype = 'float') #[angle, tof, imu, I, J] 
                    i = i+1
                except:
                    print("E")
                    continue
                
        else:
            continue
        
        # print("i=", i)
        # xi = datx [i, 10]
        # print("xi", xi)
        
        
        # yi = dat [11] 
        # xyi = int(xi + yi * sizex)
        
        # frame_dat[xyi, 2:10] = dat[2:10]
        
        
        
    ###
    datx[np.isnan(datx).any(axis=1), :] = 0 # remove nan
    datx[np.greater(abs(datx), 10000).any(axis=1), :] = 0 # remove inf
    
    imu = datx[5, 8:10]
    
    # print("tem_dat.head", tem_dat[0, :])
    print("imu x, y: {:+5.1f}, {:+5.1f}".format(imu[0], imu[1]))
    pred = model.predict(datx[:, 0:10])
    
    npt, colorz = pol2cart(datx[:, 0], datx[:, 1], pred, thre_f, thre_c)
    # pos3 = npt*ave + pos3*(1-ave)
    # color[:, 0] = colorz*ave + color[:, 0] * (1-ave)
    # color[:, 1] = (1-colorz)*ave + color[:, 1]*(1-ave)
    
    # update 
    xi = datx[:, 10]%sizex
    yi = datx[:, 11] 
    xyi = (xi + yi * sizex).astype(int)
    # pos3[xyi, :] = npt
    pos3[xyi, 2] = npt[:, 2]*ave + pos3[xyi, 2]*(1-ave)
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

