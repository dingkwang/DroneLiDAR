int p[3];
//String str;
char* cs;


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(0.2);
  p[0] = -1;
  p[1] = -1;
  p[2] = -1;
}

void loop() {


  while (!Serial.available());
  //  str = Serial.readString().toInt();
  String str = Serial.readString();
//  char str[2];
//  str[0] = cs;
//  str[1] = '\0';
//  Serial.print(str);
  //  Serial.flush();
    Serial.print("str: ");
    Serial.println(str);
  //  char cs[str.length() + 1];
  //  str.toCharArray(cs, sizeof(cs));

  //  char * token = strtok(cs, ",");
  //  p[0] = atoi(token);
  //  int i = 0;
  //
  //  while ( token != NULL ) {
  //    i = i + 1;
  //    //    Serial.print("i = ");
  //    //    Serial.println(i);
  //    token = strtok(NULL, ",");
  //    p[i] = atoi(token);
  //  }


  //  Serial.print(p[0]);
  //  Serial.print(',');
  //  Serial.print(p[1]);
  //  Serial.print(',');
  //  Serial.print(p[2]);
  //  Serial.write('\n');
  //  if (sscanf(cs, "%d,%d,%d", &p[0], &p[1], &p[2]) == 3) {
  //    // do something with r, g, b
  //    Serial.print(p[0]);
  //    Serial.print(',');
  //    Serial.print(p[1]);
  //    Serial.print(',');
  //    Serial.print(p[2]);
  //  }

  //  Serial.write(p[0]);
  //  Serial.write(p[1]);
  //  Serial.write(p[2]);
  Serial.write('\n');

  //printing each token

  //
  //  Serial.print(sscanf(cs, "%d", &x));


  //  Serial.print(cs);


}


void loop1() {
  if (Serial.available() > 0) {
    // read the incoming string:
    String incomingString = Serial.readStringUntil('\n');

    // prints the received data
    Serial.print("I received: ");
    Serial.println(incomingString);
  }
}
