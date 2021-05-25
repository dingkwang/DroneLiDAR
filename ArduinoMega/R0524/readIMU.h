#ifndef _RIMU_H
#define _RIMU_H

void read_imu_data(void);
void check_sync_byte(void);
void imu(void);
void cal_bias(void);

// Function declarations
void read_imu_data(void);
void check_sync_byte(void);
unsigned short calculate_imu_crc(byte data[], unsigned int length);

// Attitude data
union {
  float f;
  byte b[4];
} yaw; //rx
union {
  float f;
  byte b[4];
} pitch;//ry
union {
  float f;
  byte b[4];
} roll;

// Attitude data
const int bias_ave = 100;
float rx, ry;
float rxer, ryer; // ry ,rz, rx
float thx, thx_b, thy, thy_b;

// Checksum
union {
  unsigned short s;
  byte b[2];
} checksum;


// Parameters
bool imu_sync_detected = false;  // check if the sync byte (0xFA) is detected
byte in[50];  // array to save data send from the IMU

void rIMU2() {
  imu_sync_detected = false;
  // Check if new IMU data is available
  if (Serial1.available() > 4) check_sync_byte();
  // If sync byte is detected, read the rest of the data
  if (imu_sync_detected) read_imu_data();
}

void cal_bias(void) { // Acquire bias_ave=100 IMU data, average and get bias
  rx = 0;
  rxer = 0;
  ry = 0;
  ryer = 0;
  //  Serial.print("calibrating");
  delay(10);
  for (i = 0; i < bias_ave; i++) {
    rIMU2();
    rxer = yaw.f + rxer;
    ryer = roll.f + ryer;
    delay(2);
  }
  rxer = rxer / bias_ave;
  ryer = ryer / bias_ave;

  //  Serial.print("rx,");
  //  Serial.print(rxer);
  //  Serial.print(",ryer,");
  //  Serial.println(ryer);
}// cal_bias

void get_angle(void) { // Substrate the IMU real angle to local angle, sometimes have issue
  rIMU2();
  rx = yaw.f - rxer;
  ry = roll.f - ryer;
//  ry = pitch.f - ryer;
  //  Serial.print("\n get_angle,ryer,");
  //  Serial.print(rxer);
  //  Serial.print("(");
  //  Serial.print(yaw.f);
  //  Serial.print(")");
  //  Serial.print(",rx,");
  //  Serial.println(rx);
}// imu

// Check for the sync byte (0xFA)
void check_sync_byte(void) {
  for (int i = 0; i < 6; i++) {
    Serial1.readBytes(in, 1);
    if (in[0] == 0xFA) {
      imu_sync_detected = true;
      break;
    }
  }
}

// Read the IMU bytes
void read_imu_data(void) {
  Serial1.readBytes(in, 17); //41
  checksum.b[0] = in[16];
  checksum.b[1] = in[15];

  if (calculate_imu_crc(in, 15) == checksum.s) {
    for (int i = 0; i < 4; i++) {
      yaw.b[i] = in[3 + i];
      pitch.b[i] = in[7 + i];
      roll.b[i] = in[11 + i];
    }
//    Serial.println(String(yaw.f) + "," + String(pitch.f) + "," + String(roll.f));
  }
}

// Calculate the 16-bit CRC for the given ASCII or binary message.
unsigned short calculate_imu_crc(byte data[], unsigned int length)
{
  unsigned int i;
  unsigned short crc = 0;
  for (i = 0; i < length; i++) {
    crc  = (byte)(crc >> 8) | (crc << 8);
    crc ^= data[i];
    crc ^= (byte)(crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0x00ff) << 5;
  }
  return crc;
}


#endif
