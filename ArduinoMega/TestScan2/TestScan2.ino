
#define xp 3
#define yp 5
#define xm 9
#define ym 10


#include <SPI.h>
float tik= 0.031442774/1000; //ms
char buf[10];
float thxa[40], thya[40]; // max. pixels

// for motion tracking system input
union {
  float f;
  byte b[4];
} zdist;// est. target distance to LiDAR
union {
  float f;
  byte b[4];
} xAgnPosBias;
union {
  float f;
  byte b[4];
} yAgnPosBias;


int i;
int j;
int I = 0, J = 0;
int multi;
bool imu_on;
bool motion_on;

float elapsedTime, currentTime, previousTime;
float init_time, re_time;

float switch_time = 90000;
int thx_b, thy_b, thx, thy;
long dly = 0;
#include "comp_scan.h"


void setup() {
  //Additional
  TCCR2B = TCCR2B & B11111000 | B00000001;      // D9 for PWM frequency of 40k Hz
  TCCR5B = TCCR5B & B11111000 | B00000001; // D44, 45, 46
  // set MEMS acting frequency to 15kHz 

  Serial.begin(115200); //Set your serial monitor to this frequency

  // Generate full scanning pattern, px, py in comp_scan.h
  for (i = 0; i < px; i++)
  {
    thxa[i] = thx_bot + i * (thx_top - thx_bot) / (px - 1);
    thxa[2 * px - i - 1] = thxa[i] ;
  }
  for (i = 0; i < py; i++)
  {
    thya[i] = thy_bot + i * (thy_top - thy_bot) / (py - 1);
    thya[2 * py - i - 1] = thya[i] ;
  }

  //  see MEMS neutral scanning direction
  analogWrite(xp, 125); // duty cycle = 1/160 0-255
  analogWrite(xm, 125); // ~ 1/3
  analogWrite(yp, 125); // ~ 2/3
  analogWrite(ym, 125);  // 159/160

  // Print MEMS scanning range
  for (i = 0; i < 10; i++) {
    Serial.print("Settings, Angle, ");
    Serial.print(thx_top);
    Serial.print(',');
    Serial.print(thx_bot);
    Serial.print(',');
    Serial.print(thy_top);
    Serial.print(',');
    Serial.print(thy_bot);
    Serial.print(", Pixels, ");
    Serial.print(px);
    Serial.print(",");
    Serial.println(py);
  }

}// end setup

void loop() {

  comp_scan();
  //  delay(int(100000.0/tik));
  dly = 300000;
  delay(1);

}
