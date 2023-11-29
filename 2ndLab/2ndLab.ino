#include <math.h>

const int LED_PIN = 15;
const int DAC_RANGE = 4096;
const float VCC = 3.3;
const int resistor = 10000;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); //default is 10 
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
    

}

void loop() {

calculateTau();
delay(5000);
  }

float calculateLDRResistor(int analogInputValue, int dacRange){
  float quantizationInterval = VCC/dacRange;
  float luxVoltage = quantizationInterval*analogInputValue;
  float current = luxVoltage/resistor;
  float luxResistor = (VCC-luxVoltage)/current;
  return luxResistor;
}

float calculateLDRVoltage(int analogInputValue, int dacRange){
  float quantizationInterval = VCC/dacRange;
  float luxVoltage = quantizationInterval*analogInputValue;
  return luxVoltage;
}

float calculateLDRLux(float luxResistor){
  //b calculated assuming 10 LUX = 300Kohm
  float b = 6.2;
  float m = -0.86;
  float a = (log10(luxResistor)-b)/m;
  float lux = pow(10, a);
  return lux;
}

void calculateStaticGain(int divisions){
  float u[divisions+1];
  float y[divisions+1];
  int read_adc;
  float ldrResistor;
  float ldrLux;

  analogWrite(LED_PIN, 0);
  read_adc = analogRead(A0);
  ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
  ldrLux = calculateLDRLux(ldrResistor);
  float externalIllumination = ldrLux;
  
  for (int i = 0; i <= divisions; i++) {
    analogWrite(LED_PIN, i*DAC_RANGE/divisions);
    delay(100);
    u[i] = 0.1*i;
    read_adc = analogRead(A0);
    ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
    ldrLux = calculateLDRLux(ldrResistor);
    y[i] = ldrLux;
  }
  for (int i = 0; i < divisions+1; i++) {
    Serial.print("u: "); Serial.print(u[i]);Serial.print(" y: "); Serial.print(y[i]); Serial.println();
  }
}

void calculateM(){
  for (int i = 0; i <=10; i++) {
    analogWrite(LED_PIN, i*DAC_RANGE/10);
    delay(100);
    float read_adc = analogRead(A0);
    //float ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
    //float ldrLux = calculateLDRLux(ldrResistor);
    Serial.print("dc: "); Serial.print(i);Serial.print(" adc_output: "); Serial.print(read_adc); Serial.println();
  }
}

float calibrateStaticGain(){
  analogWrite(LED_PIN, 0);
  delay(100);
  float read_adc = analogRead(A0);
  float ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
  float ldrLux = calculateLDRLux(ldrResistor);
  float externalIllumination = ldrLux;
  analogWrite(LED_PIN, DAC_RANGE-1);
  delay(100);
  read_adc = analogRead(A0);
  ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
  ldrLux = calculateLDRLux(ldrResistor);
  return ldrLux-externalIllumination;
}

void calculateTau(){
  int read_adc;
  float ldrResistor;
  float ldrLux;
  float ldrVoltage;
  float sampleT= 0.001;

  for (int i = 1; i <= 4; i++) {
    analogWrite(LED_PIN, i*DAC_RANGE/4);
    delay(1000);
    read_adc = analogRead(A0); // read analog voltage
    ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
    ldrVoltage = calculateLDRVoltage(read_adc, DAC_RANGE);
    Serial.print("LDR Resistor: ");Serial.println(ldrResistor);
    //analogWrite(LED_PIN, DAC_RANGE);
    /*
    for (int j = 0; j < 400; j++) {
      read_adc = analogRead(A0); // read analog voltage
      ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
      ldrVoltage = calculateLDRVoltage(read_adc, DAC_RANGE);
      ldrLux = calculateLDRLux(ldrResistor);
      //Serial.print("LED: "); Serial.print(i*DAC_RANGE/8);
      //Serial.print(" LDR: "); 
      //Serial.print(micros());Serial.print(" ");Serial.println(ldrVoltage);
      
      //Serial.print(" V ");
      //Serial.print(ldrLux); Serial.print("LUX ");  
      //Serial.println(); 
      //delayMicroseconds(10);
      delay(1);
    }
    analogWrite(LED_PIN, 0);
    for (int j = 0; j < 400; j++) {
      read_adc = analogRead(A0); // read analog voltage
      ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
      ldrVoltage = calculateLDRVoltage(read_adc, DAC_RANGE);
      ldrLux = calculateLDRLux(ldrResistor);
      //Serial.print("LED: "); Serial.print(i*DAC_RANGE/8);
      //Serial.print(" LDR: "); 
      Serial.print(micros());Serial.print(" ");Serial.println(ldrVoltage);
      
      //Serial.print(" V ");
      //Serial.print(ldrLux); Serial.print("LUX ");  
      //Serial.println(); 
      //delayMicroseconds(10);
      delay(1);
    }*/
  }
}
