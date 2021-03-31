I am using Windows 10, ArduinoMega, Python3.7.

To run the LiDAR:
1. Open \ArduinoMega\R0314_motion_tan\R0314_motion_tan.ino with [Arduino IDE](https://www.arduino.cc/en/software), and upload the code. 
2. Install all the requirement package in requirement.txt. 
3. Collect LiDAR distance calibration data and LiDAR measurement data with read_save.py. 
4. Use model_cal.py to generate the prediction model for distance.
5. Use pred_plot.py to generate static point cloud. 
6. Use real_time_ser.py to generate realtime point cloud. 
