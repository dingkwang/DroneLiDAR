// TDC Start/Stop must have Input signal to perform calibration!!!

#define xp 44 //1
#define yp 45 //2
#define xm 9  //3 
#define ym 46 //4 

#include <SPI.h>

char buf[10]; 
float thxa[100], thya[100]; // max. pixels

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
int dly = 0;

#include "tdc.h"
#include "readIMU.h"
#include "comp_scan.h"


void setup() {
  //set pwm // set MEMS acting frequency to 15kHz 
  TCCR2B = TCCR2B & B11111000 | B00000001;      // D9 for PWM frequency of 40k Hz
  TCCR5B = TCCR5B & B11111000 | B00000001; // D44, 45, 46
  // 
  
  Serial.begin(250000); //Set your serial monitor to this frequency 115200
  // TDC set
  pinMode (CS_1, OUTPUT);
  pinMode (CS_2, OUTPUT);
  pinMode (EN, OUTPUT);
  pinMode (EN_O, OUTPUT);
  digitalWrite(EN, HIGH);
  digitalWrite (EN_O, HIGH);
  
  SPI.begin();
//  SPI.setDataMode(SPI_MODE0);//TDC7200 runs on spi mode0
  SPI.beginTransaction(SPISettings(16000000,MSBFIRST, SPI_MODE0));

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
  analogWrite(xp, 125);       // duty cycle = 1/160 0-255
  analogWrite(xm, 125); // ~ 1/3
  analogWrite(yp, 125);       // ~ 2/3
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

  // Setup IMU
  Serial1.begin(115200);// 19(RX - TX), 18(TX - RX)
  Serial1.write("$VNKMD,0*BF39\r\n"); //Enable magnet
  Serial.println("calibrating imu...");
  cal_bias();// Calculate IMU initial static value
  get_angle();// try to read IMU, expect [0, 0]
  while (abs(rx) > 0.5) { 
    Serial.println(rx);
    cal_bias();
    delay(10);
  }
  Serial.print("rxer,"); // Print IMU actual value 
  Serial.println(rxer);
  Serial.print("ryer,");
  Serial.println(ryer);
  Serial1.write("$VNKMD,1*AF18\r\n");// Disable magnet
//
  imu_on = true; // Turn on IMU MEMS compensation, otherwise MEMS not compensated scanning 
  motion_on = false; // Turn on motion based compensation  
  xAgnPosBias.f = 1.0;
  yAgnPosBias.f = 2.0;
}// end setup

void loop() {
//  read_imu_data();
  
  comp_scan();
  
//
//
  // This print the Compensated Angle = APD view angle
  Serial.print(thx_b, 1);
  Serial.print(",");
  Serial.print(thy_b, 1);
  Serial.print(",");

  delay(dly);// Wait the MEMS mirror to stablize 
//
//  i = 3;// TDC has sync issue, make sure the TDC value in range 
//  while (1) {
//    if (i <= 0 ) { // After 3 test TDC acquization, if failed, make all 0
//      tof_t1 = 0;
//      tof_t2 = 0;
//      Serial.print(tof_t1, 0);
//      Serial.print(",");
//      Serial.print(tof_t2, 0);
//      break;
//    }
//    tdc();
//    if (tof_t1 < 210 && tof_t1 > 170 && tof_t2 > 140 && tof_t2 < 200) { // Expected tof_t1, t2 range
//      tof_t1 = (tof_t1 - 170) * 10;// Encode tdc value to save printing time
//      tof_t2 = (tof_t2 - 140) * 10;
//      Serial.print(tof_t1, 0);
//      Serial.print(",");
//      Serial.print(tof_t2, 0);
//      break;
//    }
//    else i--;
//
//  }
// Print IMU data(always print)
  Serial.print(',');
  Serial.print(rx, 1);
  Serial.print(",");
  Serial.print(ry, 1);

  // Print px, py index for drawing
  Serial.print(",");
  sprintf(buf, "%02d", I);
  Serial.print(buf);
  Serial.print(',');
  sprintf(buf, "%02d", J);
  Serial.print(buf);
  //
////  Serial.print(',');
////  Serial.print(xAgnPosBias.f);
////  Serial.print(',');
////  Serial.print(yAgnPosBias.f);
////  Serial.print("\n");
}

void position() {
  Serial1.readBytes(in, 9);
  for (int i = 0; i < 4; i++) {
      xAgnPosBias.b[i] = in[3 + i];
      yAgnPosBias.b[i] = in[7 + i];
    }

}
