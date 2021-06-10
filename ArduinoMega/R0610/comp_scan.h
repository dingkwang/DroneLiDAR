#ifndef _SCAN_H
#define _SCAN_H


// Set MEMS max. optical scanning angle, +/-7 degree is the max
//float thx_top = 7;
//float thx_bot = -7;
//float thy_top = 7;
//float thy_bot = -7;

float thx_top = 3.5;
float thx_bot = -thx_top;
float thy_top = thx_top;
float thy_bot = -thx_top;


int px = 20; // {6by6, 6px6px, dly7}
int py = 20; // x2


int vx, vy; // analogy write voltage 0-255
//int vx_bias, vy_bias, vx_out, vy_out, vy_out_n;

void a2v()// Angle to voltage
{
  //  LD2 50 - 200 !!!!!!!!!!!!!!!!!
  //  vx = 124.9 - 8.0 * thx_b - 0.2 * thy_b; // Original

  //  Tripod:
//   vx = 124.9 - 4.5 * thx_b - 0.2 * thy_b;
//    vy = 129.2 - 0.45 * thx_b + 9.2 * thy_b;

// On Drone
  vx = 124.9 - 6.5 * thx_b - 2.2 * thy_b;
  vy = 130  + 0.1  * thx_b + 11.5 * thy_b;
  vx = min(200, max(50, vx));
  vy = min(200, max(50, vy));
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
    thx_b = thx  - rx; // on back
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
