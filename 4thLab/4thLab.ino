#include <Wire.h>
void setup() {
Serial.begin( 115200 ); delay( 5000 );
Wire.setSDA( 0 ); Wire.setSCL( 1 ); Wire.begin();
Wire1.setSDA( 2 );Wire1.setSCL( 3 );Wire1.begin( 0x30 );
Wire1.onReceive( recv ); Wire1.onRequest( req) ;
}

static char buff[100]; // shared memory

void recv(int len) {
int i;
for (i = 0; i < len; i++) buff[i] = Wire1.read();
buff[i] = 0;
}

void req() {
static int ctr = 765;
char buff[7];
sprintf(buff, "%06X", (ctr++) % 65535);
Wire1.write(buff, 6);
}

void loop() {
  static int p;
  char b[90];

  Serial.println("Sending...");
  Wire.beginTransmission(0x30);
  sprintf(b, "pass %d", p++);
  Wire.write(b, strlen(b));
  Wire.endTransmission();

  delay(1000);
  Serial.printf("buff: '%s'\r\n", buff);

  Wire.requestFrom(0x30, 6);
  Serial.print("\nrecv: '");
  while (Wire.available())
  Serial.print((char)Wire.read());
  Serial.println("'");
  delay(1000);
}
