#ifndef _SCAN_H
#define _SCAN_H


// Set MEMS max. optical scanning angle, +/-7 degree is the max
//float thx_top = 7;
//float thx_bot = -7;
//float thy_top = 7;
//float thy_bot = -7;

float thx_top = 4;
float thx_bot = -thx_top;
float thy_top = thx_top;
float thy_bot = -thx_top;


int px = 16; // {6by6, 6px6px, dly7}
// {6by6, 10px10px, dly1} 3.1 Hz
// {6by6, 9px9px, dly1} 312/81 Hz = 3.8Hz
//{4.5by4.5, 30px30px, dly1} 360/900 Hz
int py = px; // x2

int dly  = 0; //1

int vx, vy; // analogy write voltage 0-255
//int vx_bias, vy_bias, vx_out, vy_out, vy_out_n;

void a2v()// Angle to voltage
{

  vx = 137.5 + 8.46 * thx_b +  1.0 * thy_b;
  vy = 120.9 + 0.67 * thx_b - 9.86 * thy_b;
  vx = min(255, max(0, vx));
  vy = min(255, max(0, vy));
}

//#include "printfloat.h" // another printing formate

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
    thx_b = thx - rx;
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
