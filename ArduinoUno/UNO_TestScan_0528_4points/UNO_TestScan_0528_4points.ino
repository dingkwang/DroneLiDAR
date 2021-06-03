
#define xp 3
#define yp 5
#define xm 9
#define ym 10


#include <SPI.h>
float tik=0.031442774/1000; //ms
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
  // Configure Timer 0 for phase correct PWM @ 25 kHz.
  TCCR0A = 0;           // undo the configuration done by...
  TCCR0B = 0;           // ...the Arduino core library
  TCNT0  = 0;           // reset timer
  TCCR0A = _BV(COM0B1)  // non-inverted PWM on ch. B
           | _BV(WGM00);  // mode 5: ph. correct PWM, TOP = OCR0A
  TCCR0B = _BV(WGM02)   // ditto
           | _BV(CS00);   // prescaler = 1
  OCR0A  = 255;         // TOP = 160

  // Same for Timer 1.
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch. B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
  TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
  ICR1   = 255;

  // Same for Timer 2.
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  TCCR2A = _BV(COM2B1)  // non-inverted PWM on ch. B
           | _BV(WGM20);  // mode 5: ph. correct PWM, TOP = OCR2A
  TCCR2B = _BV(WGM22)   // ditto
           | _BV(CS20);   // prescaler = 1
  OCR2A  = 255;
  ///////////////////////

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
  dly = 600000;
  int dly2 = 200;
  delay(dly);

}
