#ifndef _TDC_H
#define _TDC_H
//http://www.ti.com/lit/ds/symlink/tdc7201.pdf
//***********************************************************************************************
//* CONFIG 1: READ: 00h; WRITE: 40h ; DATA: 1000_0001(81h)                                      *
//*           X(1: EN)  X(0:DIS)  X(0:R)   X(0:R) | X(0:RISE) XX(00:M1) X(1: START MEA)         *1000 1001
//*           FCal      PARITY    TG_E     STOP_E   STR_E     MODE      STR_MEA                 *1001 1001
//***********************************************************************************************
//* CONFIG 2: READ: 01h; WRITE: 41h ; DATA: 0101_0000(50h)                                      *
//*           XX(01:CYC10)  XXX(010:ave4)  XXX(000:R)                                          * 0100 0000
//*           Cal_P         AVG_CYC         NUM_STP                                             *
//***********************************************************************************************
//* TIME 1 : READ: 10h                                                                          *
//* COUNT 1: READ: 11h                                                                          *
//***********************************************************************************************
#define CS_1  2  //TDC1 CS
#define CS_2  4  //TDC2 CS
#define EN    8  //TDC enable
#define EN_O  7

//#include <FixedPoints.h>
//#include <FixedPointsCommon.h>

unsigned char MST_Data, SLV_Data;
unsigned char temp;

//const int startpin = 7; // DTG_trig, trig laser

unsigned long time;
int inByte;
long msb;
int mid;
int lsb;
long time1 = 0;
int count1 = 0;
long calibration1;
float timeflight;
long calibration2;
float calCount = 0;
float tof_t1;
float tof_t2;
float fehler;
const float clockperiod = 125.0; //1.0 /(8000000 / 1000000000); // (ns)Oboard ossilator is 8MHz
float normLSB1;
float normLSB2;
float c = 1; // measure the times it measures
float d = 0; //add up the time

// TDC1,2 config1,2
int cf11 = 0x99; //99: 
int cf21 = 0x89; //89
int cfx2 = 0x40; // 40:0ave; 48:2ave;  0x50:4ave;  0x60:16cyc'


void tdc() {
  //TDC-1
  //set config1
  digitalWrite(CS_1, LOW);
  SPI.transfer(0x40);
  SPI.transfer(cf11); // 99
  digitalWrite(CS_1, HIGH);
  //set config2
  digitalWrite(CS_1, LOW);
  SPI.transfer(0x41);// WRITE ADDRESS for COF2  41 for 4 average 40 for non-average
  SPI.transfer(cfx2); // SETUP for COF2
  digitalWrite(CS_1, HIGH);
  // Debug 
  // Read config1 
//  digitalWrite(CS_1, LOW);
//  SPI.transfer(0x00);
//  inByte = SPI.transfer(0);
//  digitalWrite(CS_1, HIGH);
//  Serial.print("T1 config1 = "); Serial.println(inByte, BIN);
//  // Read config2
//  digitalWrite(CS_1, LOW);
//  SPI.transfer(0x01);
//  inByte = SPI.transfer(0);
//  digitalWrite(CS_1, HIGH);
//  Serial.print("T1 config2 = "); Serial.print(inByte, BIN);


  //TDC2
  //set config1
  digitalWrite(CS_2, LOW);
  SPI.transfer(0x40); // WRITE ADDRESS for COF1
  SPI.transfer(cf21); // SETUP for COF1 91
  digitalWrite(CS_2, HIGH);
  // READ T2 config1
  //  digitalWrite(CS_2, LOW);
  //  SPI.transfer(0x00);
  //  inByte = SPI.transfer(0);
  //  digitalWrite(CS_2, HIGH);
  //  Serial.print("T2 config1 = "); Serial.println(inByte, BIN);
  //set T2 config2
  digitalWrite(CS_2, LOW);
  SPI.transfer(0x41);
  SPI.transfer(cfx2);  
  digitalWrite(CS_2, HIGH);
  // READ T2 config2
  //  digitalWrite(CS_2, LOW);
  //  SPI.transfer(0x01);
  //  inByte = SPI.transfer(0);
  //  digitalWrite(CS_2, HIGH);
  //  Serial.print("T2 config2 = "); Serial.println(inByte, BIN);
  //  digitalWrite (startpin, LOW);
  //  digitalWrite(startpin, HIGH); // laser Trig edge
  //  digitalWrite(startpin, LOW);
  //  delay(500);

  // TDC1 READ time
  //Read TDC1 Time1
  digitalWrite(CS_1, LOW);
  SPI.transfer(0x10); //Read ADDR for TIME1
  msb = SPI.transfer(0);
  mid = SPI.transfer(0);
  lsb = SPI.transfer(0);
  time1 = (msb << 16) + (mid << 8) + lsb;
  digitalWrite(CS_1, HIGH);
  //  Serial.print("TIME1_T1 = "); Serial.println(time1);
  // read TDC1 calibration1
  digitalWrite(CS_1, LOW);
  SPI.transfer(0x1B);
  msb = SPI.transfer(0);
  mid = SPI.transfer(0);
  lsb = SPI.transfer(0);
  calibration1 = (msb << 16) + (mid << 8) + lsb;
  digitalWrite(CS_1, HIGH);
  //Serial.print("CALIBRATION1_t1= "); Serial.println(calibration1, BIN);
  // read TDC1 calibration2
  digitalWrite(CS_1, LOW);
  SPI.transfer(0x1C);
  msb = SPI.transfer(0);
  mid = SPI.transfer(0);
  lsb = SPI.transfer(0);
  calibration2 = (msb << 16) + (mid << 8) + lsb;
  digitalWrite(CS_1, HIGH);
  // Serial.print("CALIBRATION2_t1= "); Serial.println(calibration2, BIN);

  float calCount = (calibration2 - calibration1) / (10.0 - 1.0); //config2 set count 10 cyc
  //Serial.print("calCount_t1= "); Serial.println(calCount);
  float normLSB1 = (clockperiod / calCount); //
  // float normLSB1 = 0.055;
  // Serial.print("normLSB1_t1= "); Serial.println(normLSB1, 16); //The length of one count, ~50 ps

  tof_t1 = normLSB1 * (time1) ; // run on TDC mode 1, count1 is always 0
  //  Serial.print("tof_t1 , ");
  //  Serial.print(tof_t1);  Serial.print(",");


  // TDC2 READ
  // read TIME1
  digitalWrite(CS_2, LOW);
  SPI.transfer(0x10);
  msb = SPI.transfer(0);
  mid = SPI.transfer(0);
  lsb = SPI.transfer(0);
  time1 = (msb << 16) + (mid << 8) + lsb;
  digitalWrite(CS_2, HIGH);
  //  Serial.print("TIME1_T2 = "); Serial.print(time1);
  // read calibration1
  digitalWrite(CS_2, LOW);
  SPI.transfer(0x1B);
  msb = SPI.transfer(0);
  mid = SPI.transfer(0);
  lsb = SPI.transfer(0);
  calibration1 = (msb << 16) + (mid << 8) + lsb;
  digitalWrite(CS_2, HIGH);
  //Serial.print("CALIBRATION1_t2 = "); Serial.println(calibration1, BIN);
  // read calibration2
  digitalWrite(CS_2, LOW);
  SPI.transfer(0x1C);
  msb = SPI.transfer(0);
  mid = SPI.transfer(0);
  lsb = SPI.transfer(0);
  calibration2 = (msb << 16) + (mid << 8) + lsb;
  //Serial.print("CALIBRATION2_t2 = "); Serial.println(calibration2, BIN);
  digitalWrite(CS_2, HIGH);
  //Serial.print("calCount = "); Serial.println(calCount);
  float normLSB2 = (clockperiod / calCount);
  //Serial.print("normLSB2_t2= "); Serial.println(normLSB2,16);
  int clockperiod = 0;
  tof_t2 = normLSB2 * float(time1);
  //Serial.print("count1_t2, "); Serial.print(count1);Serial.print(" ,");
  //Serial.print("clockperiod_t2, "); Serial.print(clockperiod);Serial.print(" ,");
  //  Serial.print(", tof_t2 , ");

  //  Serial.println(tof_t2);
}


#endif  // _TDC_H
