import numpy as np
import matplotlib.pyplot as plt
import joblib
from sklearn.metrics import mean_squared_error

def prep(file_name):
    gft = (np.genfromtxt(file_name,delimiter=','))[1:, :] # generate from text
    gft = gft[~np.isnan(gft).any(axis=1)] # remove nan
    
    return gft


#%%
path = ""
fn1 =  'test1.csv'

modelname_1 = 'model_i.sav'

dat_1 = prep(fn1)

start, stop = 0, 2000
thre_f = 400 # Est. background position(cm)
thre_c = 100 # Est. target position(cm)


#%% Plot IMU 
fig = plt.figure(1)
t = np.arange(len(dat_1))/400
plt.plot(t, dat_1[:, 4], label = "IMU_x")
plt.plot(t, dat_1[:, 5], label = "IMU_y")

plt.xlabel("Time(s)")
plt.ylabel("Angle(°)")
plt.legend()
plt.title(fn1 + "_IMU")

#%% Predict with model
model_1 = joblib.load(modelname_1)
pred_1 = model_1.predict(dat_1[:, :4])

mean_pred = np.mean(pred_1)

# rmse = np.sqrt(mean_squared_error(pred_1, mean_pred * np.ones(np.shape(pred_1))))
# print("root Mean squared error: %.2f" % rmse)
#%%

# 'Angle X, Angle Y, Pred distance '

def pol2cart(thx, thy, d, thre_f, thre_c):
    # Optional, data filter to decrease noise 
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
    
    return x, y, z
#%% Get one frame
def get_frame(dat_1, pred_1, beg, end): # size of one frame 
    return dat_1[beg:end, :], pred_1[beg:end] 


dat_1, pred_1 = get_frame(dat_1, pred_1, start, stop)


# Change if the point cloud is flipped
thx = -dat_1[:, 0]
thy = dat_1[:, 1] # Off

dist = pred_1



x1, y1, z1 = pol2cart(thx, thy, dist, thre_f, thre_c)

#%% 
fig = plt.figure(3)
ax = plt.axes(projection='3d')
im = ax.scatter3D(x1, y1, z1, c=z1, cmap='RdBu', s = 10)
ax.set_xlabel('X(cm)')
ax.set_ylabel('Y(cm)')
ax.set_zlabel('Z(cm)')

# ax.set_zticks(np.arange(thre_c-1, thre_f+1, 2))

cbar = plt.colorbar(im)
cbar.ax.set_ylabel('Z (cm)')
ax.set_title(fn1 + " One Frame", y=-0.15)
plt.tight_layout()


#%%
# '''Average MultiFrame for static scene''' 

dp1 = np.concatenate((dat_1[:, 0:2], pred_1.reshape(-1 ,1)), axis = 1)
def ave(dp, thre_f, thre_c):
    of = np.unique(dp[:, 0:2], axis = 0)
    of = np.hstack((of, np.ones((len(of), 1))))
    for i in range(len(of)):
        iloc =  np.where((dp[:, 0:1] == of[i, 0:1]) & (dp[:, 1:2]==of[i, 1:2] ))
        pm = np.mean(dp[iloc[0], -1])
        of[i, -1] = pm
#    of = dp
    offset = 1
    x, y, z = pol2cart(of[:, 0], of[:, 1], of[:, 2]* offset, thre_f, thre_c)
    return x, y, z

# thre_f = 180
# thre_c = 150
xa1, ya1, za1 = ave(dp1, thre_f, thre_c)


fig = plt.figure(4)
ax = plt.axes(projection='3d')
im = ax.scatter3D(xa1, ya1, za1, c=za1, cmap='RdBu', s = 20) # 'cool'
cbar = plt.colorbar(im)
cbar.ax.set_ylabel('Z (cm)')

ax.set_xlabel('X(cm)')
ax.set_ylabel('Y(cm)')
ax.set_zlabel('Z(cm)')


ax.set_title('averaged_'+ fn1, y=-0.15) # increase or decrease y as needed





