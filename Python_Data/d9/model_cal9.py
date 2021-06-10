# Spyder has error 

import numpy as np
from sklearn.metrics import mean_squared_error
import matplotlib.pyplot as plt
import glob
import joblib

from os import chdir

# chdir(r"Calibration_Data_030621/")

#%%
def prep(file_name, calib):
    print(file_name)
    gft = (np.genfromtxt(file_name,delimiter=','))[1:, :] # generate from text
    y = np.ones([len(gft), 1]) * int(file_name[1:-4]) # Correct filename 
    gft = np.hstack((gft, y)) # with distance at end 
   
    
    gft = gft[~np.isnan(gft).any(axis=1)] # remove nan
    
    x = gft[:, 0:4] # MEMS angle + tof1,2
    y = gft[:, -1]

    return x, y

filename = "c*.csv"

x = np.empty([1, 4]) 

y = np.empty([1])
for file_name in glob.glob(filename):
    temx, temy = prep(file_name, 1)
    x = np.append(x, temx, axis = 0)
    y = np.append(y, temy, axis = 0)
    
    x = x [1:] # abandon some initial data
    y = y [1:]

#%%

from sklearn.model_selection import train_test_split
xtrain, xtest, dtrain, dtest = train_test_split(x, y, test_size=0.1, random_state=15)

from sklearn.ensemble import RandomForestRegressor
regr = RandomForestRegressor(max_depth=15, random_state=1,n_estimators=100)

plt.figure(1)
plt.plot(x[:, 2:4])
plt.xlabel('Samples')
plt.ylabel('Tof(ns)')
plt.title('raw Data')

#%%
regr.fit(xtrain, dtrain)

filename = 'model_i.sav'
joblib.dump(regr, filename)

pred = regr.predict(xtest)
rslt = np.vstack((pred, dtest))
#print("pred vs actual ", rslt)
print("Root Mean squared error: %.2f"
      % np.sqrt(mean_squared_error(dtest, pred)))

plt.figure(3)
plt.scatter(dtest,pred)
plt.xlabel('Actual Distance(cm)')
plt.ylabel('Predicted Distance(cm)')
plt.title('Validation Set')

