#include <math.h>

const int LED_PIN = 15;
const int DAC_RANGE = 4096-1;
const float VCC = 3.3;
const int resistor = 10000;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); //default is 10 
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
}

void loop() {
  delay(5000);
  calculateTau();

}

void calculateTau(){
  int read_adc;
  long start = micros();

  analogWrite(LED_PIN, 0);
  delay(1000);
  read_adc = analogRead(A0); // read analog voltage
  Serial.print(micros()-start);Serial.print(" ");Serial.println(read_adc);
  
  for (int i = 1; i <= 4; i++) {
    analogWrite(LED_PIN, i*DAC_RANGE/4);
    for (int j = 0; j < 3000; j++) {
      read_adc = analogRead(A0); // read analog voltage
      Serial.print(micros()-start);Serial.print(" ");Serial.println(read_adc);
      delayMicroseconds(5);
    }
    
    delay(1000);
    read_adc = analogRead(A0); // read analog voltage
    Serial.print(micros()-start);Serial.print(" ");Serial.println(read_adc);
    
    analogWrite(LED_PIN, 0);
    for (int j = 0; j < 3000; j++) {
      read_adc = analogRead(A0); // read analog voltage
      Serial.print(micros()-start);Serial.print(" ");Serial.println(read_adc);
      delayMicroseconds(5);
    }
    delay(1000);
    read_adc = analogRead(A0); // read analog voltage
    Serial.print(micros()-start);Serial.print(" ");Serial.println(read_adc);
  }
}
