#include <simulator.h>
#include <math.h>

const int LED_PIN = 15;
const float DAC_RANGE = 4096;
const float VCC = 3.3;
const int resistor = 10000;
const float samp_time {0.01}; //10ms
int reference {1};
float staticalGain;
float externalIlluminance;
String inString = ""; // string to hold input
simulator controller;

void setup() {
Serial.begin(115200);
    analogReadResolution(12);    // default is 10
    analogWriteFreq(60000);      // 60KHz, about max
    analogWriteRange(DAC_RANGE); 
    calibrateStaticGain();
    analogWrite(LED_PIN, 0);
    delay(1000);
}

void loop() {
  
  float vi, y, vf = 0;
  long start;
  int read_adc = 0;
  analogWrite(LED_PIN, 0);
  delay(2000);
  
  int adc = analogRead(A0);
  vi = (3.3/4096*adc);
    controller.start(staticalGain,vi, micros());
    reference = 60.0;
    analogWrite(LED_PIN, int(reference/staticalGain * 4096.0));
    for(int j = 0; j<3000; j++){
      read_adc = analogRead(A0);
      Serial.print(adc2lux(read_adc));
      Serial.print(" ");
      vi = controller.simulate(micros(), reference);
      Serial.println(controller.volt2lux(vi));
      delayMicroseconds(10);
    }
    
}


void measureExternalIllumination()
{
    analogWrite(LED_PIN, 0);
    delay(1000);
    int read_adc = 0;
    for(int i = 0; i<3; i++){
      read_adc+=analogRead(A0);
      }
    read_adc = int(float(read_adc)/3.0);
    externalIlluminance = adc2lux(read_adc);
}
void calibrateStaticGain()
{
    measureExternalIllumination();
    analogWrite(LED_PIN, DAC_RANGE);
    delay(1000);
    int read_adc = 0;
    for(int i = 0; i<3; i++){
      read_adc+=analogRead(A0);
      }
    read_adc = int(float(read_adc)/3.0);
    staticalGain = adc2lux(read_adc) - externalIlluminance;
}

float adc2lux(int adc){
  float a = (3.3/4096.0*float(adc));
  float luxResistor = (3.3-a)*10000.0/a;
  float lux = pow(10.0, (log10(luxResistor)-6.2)/-0.86);
  return lux;
}
