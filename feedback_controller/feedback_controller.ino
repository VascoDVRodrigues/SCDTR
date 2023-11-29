#include <metrics.h>
#include <controller.h>
#include <math.h>

const int LED_PIN = 15;
const float DAC_RANGE = 4096.0;
const float samp_time {10}; //10ms
float reference {10.0};
float staticalGain;
float externalIlluminance;
String inString = ""; // string to hold input
const float p_gain {0.0};
const float i_gain {0.4};
controller basco;
metrics metric;
void setup() {
  Serial.begin(115200);
  analogReadResolution(12); //default is 10 
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
  calibrateStaticGain();
  basco.setup(staticalGain, samp_time, p_gain, i_gain);
  basco.setReference(10.0);
  metric.setTime(long(micros()));
  metric.setDutyCycle(0.0);
}

void loop() { 
  metric.updateEnergy(long(micros()));
  int read_adc = analogRead(A0);
  float y = adc2lux(read_adc);
  int pwm = basco.calculate(y);
  metric.sampleVisibilityError(reference, y);
  metric.sampleInstantFlicker(y);
  analogWrite(LED_PIN, pwm);
  metric.setDutyCycle(float(pwm)/DAC_RANGE);
  
  Serial.print(reference);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.print(100);
  Serial.print(" ");
  Serial.println(y);
  //Serial.print(" ");
  //Serial.print(metric.getAvgVisibilityError());
  //Serial.print(" ");
  //Serial.println(metric.getFlickerError());
  delay(samp_time);
}
void loop1(){
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    if (inChar == '\n') { //got newline, now process input
      reference = float(inString.toInt()); // print value
      inString = "";
      basco.setReference(reference);
    }
  }
}

void calibrateStaticGain(){
  analogWrite(LED_PIN, 0);
  delay(1000);
  float read_adc = analogRead(A0);
  externalIlluminance = adc2lux(read_adc);
  analogWrite(LED_PIN, DAC_RANGE);
  delay(1000);
  read_adc = analogRead(A0);
  staticalGain = adc2lux(read_adc)-externalIlluminance;
}
  
float adc2lux(int adc){
  float a = (3.3/4096.0*float(adc));
  float luxResistor = (3.3-a)*10000.0/a;
  float lux = pow(10.0, (log10(luxResistor)-6.2)/-0.86);
  return lux;
} 
