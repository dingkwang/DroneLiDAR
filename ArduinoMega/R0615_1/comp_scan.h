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


int px = 10; // {6by6, 6px6px, dly7}
int py = 10; // x2


int vx, vy; // analogy write voltage 0-255
//int vx_bias, vy_bias, vx_out, vy_out, vy_out_n;

void a2v()// Angle to voltage
{
  // M2 -3V Org
  //  vx = 132.9 + 9.5 * thx_b + 1.0 * thy_b;
  //  vy = 126.9 - 1.0 * thx_b + 10.5 * thy_b;
  // 0610
  vx = 132.9 + 20.5 * thx_b + 1.5 * thy_b;
  vy = 126.9 - 1.0 * thx_b + 25.0 * thy_b;

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
    thx_b = thx - rx; // on back
    thy_b = thy - ry;
  }
  else {
    thx_b = thx ;
    thy_b = thy ;
  }
  
  if (translation_on) {
    thx_b = thx_b - azimuth_angle * 180.0 / 3.1415926;
    thy_b = thy_b + elevation_angle * 180.0 / 3.1415926;
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
