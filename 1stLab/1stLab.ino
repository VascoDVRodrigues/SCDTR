#include <math.h>

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float VCC = 3.3;
const int resistor = 10000;
int counter = 0;
// the setup function runs once 
void setup() {
  Serial.begin(115200);
  analogReadResolution(12); //default is 10 
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
  
}
// the loop function runs over and over again 

void loop() {
  int read_adc;
  //analogWrite(LED_PIN, counter); // set led PWM
  delay(500); //delay 1ms
  read_adc = analogRead(A0); // read analog voltage
  //counter = counter + 1;
  //if (counter > DAC_RANGE) // if counter saturates 
  //  counter = 0; // reset counter

  float ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
  float ldrLux = calculateLDRLux(ldrResistor);
  Serial.print(read_adc); Serial.print(", "); 
  Serial.print(ldrResistor); Serial.print(" ohm, ");
  Serial.print(ldrLux); Serial.print("LUX ");  
  Serial.println(); 
}

float calculateLDRResistor(int analogInputValue, int dacRange){
  float quantizationInterval = VCC/dacRange;
  float luxVoltage = quantizationInterval*analogInputValue;
  float current = luxVoltage/resistor;
  float luxResistor = (VCC-luxVoltage)/current;
  return luxResistor;
}

float calculateLDRLux(float luxResistor){
  int b = 6.53;
  float m = -0.8;
  float a = (log10(luxResistor)-b)/m;
  float lux = pow(10, a);
  return lux;
}
