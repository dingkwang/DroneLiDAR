<<<<<<< HEAD
#ifndef _SCAN_H
#define _SCAN_H


// Set MEMS max. optical scanning angle, +/-7 degree is the max
//float thx_top = 7;
//float thx_bot = -7;
//float thy_top = 7;
//float thy_bot = -7;

float thx_top = 0;
float thx_bot = -thx_top;
float thy_top = thx_top;
float thy_bot = -thx_top;


int px = 20; // {6by6, 6px6px, dly7}
int py = 20; // x2


int vx, vy; // analogy write voltage 0-255
//int vx_bias, vy_bias, vx_out, vy_out, vy_out_n;

void a2v()// Angle to voltage
{
  // M1 -3V
  //  vx = 131.7 - 8.67 * thx_b - 0.23 * thy_b;
  //  vy = 128.2 + 0.08 * thx_b - 12.0 * thy_b;
  // M2 -3V
  vx = 132.9 + 9.5 * thx_b + 1.0 * thy_b;
  vy = 126.9 - 1.0 * thx_b + 10.5 * thy_b;
  
//  vx = 132.9 + 9.2 * thx_b + 1.0 * thy_b;
//  vy = 126.9 - 1.0 * thx_b + 10.3 * thy_b;
  // M3 -2V
//  vx = 124.5 -12.4 * thx_b -1.86 * thy_b;
//  vy = 129.6 + 1.3 * thx_b -15.1 * thy_b; 
//  vx = thx_b;
//  vy = thy_b;
  vx = min(255, max(0, vx));
  vy = min(255, max(0, vy));
}

void comp_scan()
{
  // Generate raster z scan 
  if (I >= px * 2 - 1) //update vx
    I = 0;
  else I++;

  if (I == 0 || I == px) //update vy
  {
    if (J >= py * 2 - 1) J = 0;
    else J++;
  }
  // Horizontal
  thx = thxa[I];
  thy = thya[J];

  get_angle(); // Get IMU data
  
  if (imu_on) {
    thx_b = thx  + rx;
    thy_b = thy - ry;
  }
  else {
    thx_b = thx ;
    thy_b = thy ;
  }

  if (motion_on) {
    thx_b = thx_b - xAgnPosBias.f;
    thy_b = thy_b - yAgnPosBias.f;
  }
  
  a2v();
  //  Serial.print(",vx,");
  //  Serial.print(vx);
  //  Serial.print(",vy,");
  //  Serial.print(vy);
  //  Serial.print(",");

  // Output to pins
  analogWrite(xp, vx);       // duty cycle = 1/160 0-255
  analogWrite(xm, 255 - vx); // ~ 1/3
  analogWrite(yp, vy);       // ~ 2/3
  analogWrite(ym, 255 - vy);  // 159/160

}

#endif  // _SCAN_H
=======
#ifndef _SCAN_H
#define _SCAN_H


// Set MEMS max. optical scanning angle, +/-7 degree is the max
//float thx_top = 7;
//float thx_bot = -7;
//float thy_top = 7;
//float thy_bot = -7;

float thx_top = 0;
float thx_bot = -thx_top;
float thy_top = thx_top;
float thy_bot = -thx_top;


int px = 20; // {6by6, 6px6px, dly7}
int py = 20; // x2


int vx, vy; // analogy write voltage 0-255
//int vx_bias, vy_bias, vx_out, vy_out, vy_out_n;

void a2v()// Angle to voltage
{
  // M1 -3V
  //  vx = 131.7 - 8.67 * thx_b - 0.23 * thy_b;
  //  vy = 128.2 + 0.08 * thx_b - 12.0 * thy_b;
  // M2 -3V
  vx = 132.9 + 9.5 * thx_b + 1.0 * thy_b;
  vy = 126.9 - 1.0 * thx_b + 10.5 * thy_b;
  
//  vx = 132.9 + 9.2 * thx_b + 1.0 * thy_b;
//  vy = 126.9 - 1.0 * thx_b + 10.3 * thy_b;
  // M3 -2V
//  vx = 124.5 -12.4 * thx_b -1.86 * thy_b;
//  vy = 129.6 + 1.3 * thx_b -15.1 * thy_b; 
//  vx = thx_b;
//  vy = thy_b;
  vx = min(255, max(0, vx));
  vy = min(255, max(0, vy));
}

void comp_scan()
{
  // Generate raster z scan 
  if (I >= px * 2 - 1) //update vx
    I = 0;
  else I++;

  if (I == 0 || I == px) //update vy
  {
    if (J >= py * 2 - 1) J = 0;
    else J++;
  }
  // Horizontal
  thx = thxa[I];
  thy = thya[J];

  get_angle(); // Get IMU data
  
  if (imu_on) {
    thx_b = thx  + rx;
    thy_b = thy - ry;
  }
  else {
    thx_b = thx ;
    thy_b = thy ;
  }

  if (motion_on) {
    thx_b = thx_b - xAgnPosBias.f;
    thy_b = thy_b - yAgnPosBias.f;
  }
  
  a2v();
  //  Serial.print(",vx,");
  //  Serial.print(vx);
  //  Serial.print(",vy,");
  //  Serial.print(vy);
  //  Serial.print(",");

  // Output to pins
  analogWrite(xp, vx);       // duty cycle = 1/160 0-255
  analogWrite(xm, 255 - vx); // ~ 1/3
  analogWrite(yp, vy);       // ~ 2/3
  analogWrite(ym, 255 - vy);  // 159/160

}

#endif  // _SCAN_H
>>>>>>> 764a81c2bf64be665eb68167d7d1596dae8c1d4d
