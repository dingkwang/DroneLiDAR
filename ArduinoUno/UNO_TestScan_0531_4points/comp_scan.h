#ifndef _SCAN_H
#define _SCAN_H

// Set MEMS max. optical scanning angle, +/-7 degree is the max
float thx_top = 7;
float thx_bot = -thx_top;
float thy_top = thx_top;
float thy_bot = -thx_top;

// For calibration
//float thx_top = 255;
//float thx_bot = 128;
//float thy_top = 128;
//float thy_bot = 0;


int px = 20; // {6by6, 6px6px, dly7}
int py = px; // x2


int vx, vy; // analogy write voltage 0-255
//int vx_bias, vy_bias, vx_out, vy_out, vy_out_n;

void a2v()// Angle to voltage
{

  //  // M3 -2V
  //  vx = 124.5 -12.4 * thx_b -1.86 * thy_b;
  //  vy = 129.6 + 1.3 * thx_b -15.1 * thy_b;
  // LD2 50-200 !!!!!!!!!!!!!!!!!
//    vx = 124.9 - 8.0 *thx_b - 0.2*thy_b;
//    vy = 129.2 - 0.45*thx_b + 9.2*thy_b;
//    vx = min(200, max(50, vx));
//    vy = min(200, max(50, vy));
//   LD1 50-200 !!!!!!!!!!!!
//    vx = 124.9 - 7.3*thx_b - 0.7*thy_b;
//    vy = 131.0 - 0.78*thx_b - 8.4*thy_b;
  
  // LSF1
//  vx = 126.2 + 1.5 * thx_b - 6.9*thy_b;
//  vy = 124.1 - 6.6 * thx_b - 1.6* thy_b;
//  vx = min(255, max(0, vx));
//  vy = min(255, max(0, vy));

// LSF2
//  vx = 129.1 + 1.9*thx_b - 6.1*thy_b;
//  vy = 129.4 +6.0*thx_b + 2.25*thy_b;
//  vx = min(205, max(50, vx));
//  vy = min(205, max(50, vy));
  // Lsfb -3.3V, slow mirror 
    vx = 124.3 -13.4*thx_b + 0.0001*thy_b;
    vy = 132.0 - 0.22*thx_b + 16.5*thy_b;
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


  thx_b = thx;
  thy_b = thy;


  a2v();
  Serial.print("vx,");
  Serial.print(vx);
  Serial.print(",vy,");
  Serial.print(vy);
  Serial.print(",\n");

  // Output to pins
  analogWrite(xp, vx);       // duty cycle = 1/160 0-255
  analogWrite(xm, 255 - vx); // ~ 1/3
  analogWrite(yp, vy);       // ~ 2/3
  analogWrite(ym, 255 - vy);  // 159/160

}

#endif  // _SCAN_H
