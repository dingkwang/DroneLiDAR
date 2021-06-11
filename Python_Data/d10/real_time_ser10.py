
import numpy as np

from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import random
import serial
import joblib

from time import strftime


modelname = 'model_i.sav'

sizex = 20
sizey = sizex
sizeall = sizex * sizey

time = strftime("%d_%H_%M")

fn = 'Data_' + time + '_c.csv'
file = open(fn, 'w')

sercom = "/dev/ttyACM0"
model = joblib.load(modelname)
ser=serial.Serial(sercom,115200)

def Sergetline(ser):
    while (1):
        line= ser.readline()
        # print(len(line), ', ', line)
        # print("lineLength = ", len(line))
        if len(line) >= 35 and len(line) <= 42 :
            # print(line)
            try:
                 a = line.decode("ascii")
                 lst = a.split(',')
                 dat = [float(i) for i in lst]
                 if len(dat) == 8:
                     file.write(a)
                     return dat
                 else:
                     Sergetline(ser)
            except: 
                Sergetline(ser)

        else: 
             line= ser.readline()

def pol2cart(thx, thy, d, thre_f, thre_c):
    # thx= -(thx - 125)/255*6
    # thy = -(thy- 125)/255*6

    d = d
    for i in range(len(d)):
        if d[i] > thre_f:
            d[i] = thre_f + np.random.rand()/2
        elif d[i] < thre_c:
            d[i] = thre_c + np.random.rand()/2

    thxd, thyd = -np.radians(thx), np.radians(thy)
    dz = np.sqrt(1/(np.cos(thxd))**2+1/(np.cos(thyd))**2-1)
    z = d/dz
    x = z / np.cos(thy) * np.tan(thx)
    x = np.sqrt(abs(d**2- (z/np.cos(thyd))**2)) * np.sign(thx)
    y = np.sqrt(abs(d**2- (z/np.cos(thxd))**2)) * np.sign(thy)
    

    zm = thre_f+10-z
    # zm = z
    npt = np.transpose(np.array([x, y, zm]))
    colorz = (z - thre_c+2)/(thre_f-thre_c+2)
    
    
    return npt, colorz

def mapP(xi, yi, sizex, sizey):
    xi = np.array(xi)
    yi = np.array(yi)
    np.subtract(2 * sizex - 1, xi, out = xi, where = xi > sizex-1 )
    np.subtract(2 * sizey - 1, yi, out = yi, where = yi > sizey-1 )
       
    return xi, yi

frame_dat = np.zeros((sizeall, 8), dtype="float") ## store raw data[Scanx, scanY, tof1, tof2]

pos3 = np.empty((sizex*sizey,3))


color = np.empty((len(pos3),4), dtype=np.float32)

color[:,0] = 1
color[:,1] = 1
color[:,2] = 1
color[:,3] = 1

thre_f = 500
thre_c = 100

#Collect whole frame data 
xyi = 0
xi, yi = -1, -1

print("Collecting first frames...")
i = 0



while(i<sizeall+2):
    line_dat = Sergetline(ser)
    xi = int(line_dat [6])
    yi = int(line_dat[7])
  
    xi, yi = mapP(xi, yi, sizex, sizey)

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
w.setFixedSize(400,400)
# w.setCameraPosition(distance=120, elevation = 90, azimuth=270)
w.setCameraPosition(distance=250, elevation = 90, azimuth=270)

g = gl.GLGridItem()
w.addItem(g)

sp3 = gl.GLScatterPlotItem(pos=pos3, color=color, size=1.5, pxMode=False)

sp3.setData(pos=pos3, color=color)
w.addItem(sp3)




t = 0

stamp = sizex
# stamp = 1
datx = np.zeros((stamp,len(line_dat)))
ave = 0.9

#%%
def update():
    global sp3, pos3, npt, model, t, color, sizeall, sizex, thre_f, thre_c, frame_dat, datx
    global ave
    i = 0
    # with serial.Serial("com5", 115200) as ser:  
    # while(i<(sizex)):   
    while(i<(stamp)):         
        line_dat = Sergetline(ser)
        datx[i] = line_dat
        i = i + 1
    
    # print(datx[:, 6:8])
    
    imu = datx[-1, 4:6]
    print("imu x, y: {:+5.1f}, {:+5.1f}".format(imu[0], imu[1]))
    pred = model.predict(datx[:, :4])
    
    new_pt, colorz = pol2cart(datx[:, 0], datx[:, 1], pred, thre_f, thre_c)
    
    new_pt[np.less(new_pt[:, 2:], 0.1).any(axis=1), :] = np.nan
    # pos3 = npt*ave + pos3*(1-ave)
    # color[:, 0] = colorz*ave + color[:, 0] * (1-ave)
    # color[:, 1] = (1-colorz)*ave + color[:, 1]*(1-ave)
    
    xi = datx[:, 6]
    yi = datx[:, 7] 
   
    # update 
    xi, yi = mapP(xi, yi, sizex, sizey)
    xyi = (xi + yi * sizex).astype(int)
    

    pos3[xyi, 2] = new_pt[:, 2]  *ave + pos3[xyi, 2]*(1-ave)
    
    color[xyi, 0] = colorz*ave + color[xyi, 0] * (1-ave)
    color[xyi, 1] = (1-colorz)*ave  + color[xyi, 1]*(1-ave)

    sp3.setData(pos=pos3,color = color)
    app.processEvents()
    
timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(0)

    
if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

