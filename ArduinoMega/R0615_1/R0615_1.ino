// TDC Start/Stop must have Input signal to perform calibration!!!

#define xp 44 //1
#define yp 45 //2
#define xm 9  //3 
#define ym 46 //4 

#include <SPI.h>

bool isTDC = 0;
bool printPXY = 0;
bool printIMU = 1;
bool isScan = printIMU;

char buf[10];
float thxa[100], thya[100]; // max. pixels


float TargetX = 1.24;
float TargetY = -0.175;
float TargetZ = 0.78;
float azimuth_angle = 0.0;
float elevation_angle = 0.0;


int i;
int j;
int I = 0, J = 0;
int multi;
bool imu_on  = 1;
bool motion_on = 1;

float elapsedTime, currentTime, previousTime;
float init_time, re_time;
int dly = 0;

bool read_from_python = 0;

#include "tdc.h"
#include "readIMU.h"
#include "comp_scan.h"


void setup() {
  //set pwm // set MEMS acting frequency to 15kHz
  TCCR2B = TCCR2B & B11111000 | B00000001;      // D9 for PWM frequency of 40k Hz
  TCCR5B = TCCR5B & B11111000 | B00000001; // D44, 45, 46
  Serial3.begin (115200);
  Serial2.begin (115200);
  Serial.begin(115200); //Set your serial monitor to this frequency 115200
  // TDC set
  pinMode (CS_1, OUTPUT);
  pinMode (CS_2, OUTPUT);
  pinMode (EN, OUTPUT);
  pinMode (EN_O, OUTPUT);
  digitalWrite(EN, HIGH);
  digitalWrite (EN_O, HIGH);

  SPI.begin();
  //  SPI.setDataMode(SPI_MODE0);//TDC7200 runs on spi mode0
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

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

}// end setup

void loop() {

  if (isScan) {
    comp_scan();
    // This print the Compensated Angle = APD view angle
    Serial.print(thx_b, 1);
    Serial.print(",");
    Serial.print(thy_b, 1);
    Serial.print(",");
  }

  delay(dly);// Wait the MEMS mirror to stablize

  //
  if (isTDC) {
    i = 3;// TDC has sync issue, make sure the TDC value in range
    while (1) {
      if (i <= 0 ) { // After 3 test TDC acquization, if failed, make all 0
        tof_t1 = 0;
        tof_t2 = 0;
        Serial.print(tof_t1, 0);
        Serial.print(",");
        Serial.print(tof_t2, 0);
        break;
      }
      tdc();
      if (tof_t1 < 250 && tof_t1 > 160 && tof_t2 > 150 && tof_t2 < 250 ) { // Expected tof_t1, t2 range
        //    if (1) {
        //tof_t1 = (tof_t1 - 150) * 10;// Encode tdc value to save printing time
        //tof_t2 = (tof_t2 - 140) * 10;
        Serial.print(tof_t1);
        Serial.print(",");
        Serial.print(tof_t2);
        break;
      }
      else i--;
    }
  } // isTDC

  // Print IMU data(always print)
  if (printIMU) {
    Serial.print(',');
    Serial.print(rx, 1);
    Serial.print(",");
    Serial.print(ry, 1);
  }

  // Print px, py index for drawing
  if (printPXY) {
    Serial.print(",");
    sprintf(buf, "%02d", I);
    Serial.print(buf);
    Serial.print(',');
    sprintf(buf, "%02d", J);
    Serial.print(buf);
  }

  read_vicon();
  aim_at_target(TargetX, TargetY, TargetZ);
  Serial.print("Azimuth: ");
  Serial.print(azimuth_angle * 180.0 / 3.1415926);
  Serial.print(",");
  Serial.print("Elevation: ");
  Serial.print(elevation_angle * 180.0 / 3.1415926);
  //azimuth_angle

  Serial.print("\n");

}



float x, y, z; //vicon position, updated by read_vicon(), used in aim_at_target()
char *field;  //temporary pointer

size_t         count     = 0;
const size_t   MAX_CHARS = 64;
char           line[ MAX_CHARS ];


void read_vicon() {  //use in loop()

  if (lineReady( Serial2 )) {
    field = strtok( line, "," );
    x = atof( field );
    field = strtok( NULL, "," );
    y = atof( field );
    field = strtok( NULL, "," );
    z = atof( field );
  }
  //    Serial.print(",position,");
  //    Serial.print(x);
  //    Serial.print(",");
  //    Serial.print(y);
  //    Serial.print(",");
  //    Serial.print(z);
  //Serial.print("\n");
}

bool lineReady( Stream & input )    // could be Serial or ethernet client
{
  bool          ready     = false;
  const char    endMarker = '\n';

  while (input.available()) {
    char c = input.read();
    if (c != endMarker) {
      // Only save the printable characters, if there's room
      if ((' ' <= c) and (count < MAX_CHARS - 1)) {
        line[ count++ ] = c;
      }
    } else {
      //  It's the end marker, line is completely received
      line[count] = '\0'; // terminate the string
      count       = 0;    // reset for next time
      ready       = true;
      break;
    }
  }
  return ready;
} // lineReady




void aim_at_target(float TargetX, float TargetY, float TargetZ)
{
  //  TargetX = 15.0; //hard coding right now, can comment
  //  TargetY = 15.0;
  //  TargetZ = -1.0;
  float vectorx = TargetX - x; //x,y,z is the lidar's position
  float vectory = TargetY - y;
  float vectorz = TargetZ - z;
  azimuth_angle = atan2(vectory, vectorx);
  elevation_angle = atan2(vectorz, sqrt(pow(vectorx, 2) + pow(vectory, 2)));
}
