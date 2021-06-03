int p[3];
//String str;
char* cs;

union {
  int i;
  byte b[4];
} yaw; //rx
union {
  int i;
  byte b[4];
} pitch;//ry
union {
  int i;
  byte b[4];
} roll;


//union {
//  byte b[6];
//  int i;
//} in;
byte in[6];

union {
  unsigned short s;
  byte b[2];
} checksum;

bool sync_detected;

int len = 4;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(0.2);
  p[0] = -1;
  p[1] = -1;
  p[2] = -1;

}



void loop() {

  yaw.i = -1;
  pitch.i = 2;
  roll.i = 3;
  sync_detected = false;
  if (Serial.available() > 4) check_sync_byte();

  if (sync_detected) {
    Serial.readBytes(in, 6);
    checksum.b[0] = in[5];
    checksum.b[1] = in[4];
    
    unsigned short s = calculate_imu_crc(in, len);
    Serial.print("s: " + s);
    if (s == checksum.s) {
      for (int i = 0; i < 4; i++) {
        yaw.b[i] = in[0 + i];
        //   in[7 + i] = pitch.b[i];
        //   in[11 + i] = roll.b[i];
      }
      Serial.print("sync_detected ");
      Serial.print(sync_detected + '\0');
      Serial.println(yaw.i);
    }

    
  }
}

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

void check_sync_byte(void) {
  for (int i = 0; i < 6; i++) {
    Serial.readBytes(in, 1);
    if (in[0] == 0x0F) {
      sync_detected = true;
      break;
    }
  }
}
