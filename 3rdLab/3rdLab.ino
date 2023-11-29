#include <simulator.h>
#include <math.h>

const int LED_PIN = 15;
const int DAC_RANGE = 4096-1;
const float VCC = 3.3;
const int resistor = 10000;
float p_gain {0.0};
const float samp_time {0.01}; //10ms
int reference {250};
float staticalGain = 98;
float externalIlluminance = 0;
String inString = ""; // string to hold input
simulator controller{1,2,3,4};

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); //default is 10 
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
  analogWrite(LED_PIN, 0);
  delay(1000);
  calibrateStaticGain();
  controller.setup1(staticalGain, externalIlluminance);
  int adc = analogRead(A0);
  controller.start((3.3/4096*adc), long(micros()));
  //analogWrite(LED_PIN, lux2adc(reference));
  delay(2000);
}

void loop() {
  
  
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    if (inChar == '\n') { //got newline, now process input
      reference = inString.toInt(); // print value
      inString = "";
      int adc = analogRead(A0);
      controller.start((3.3/4096*adc), long(micros()));
    }
  }
  
    FeedforwardControl();
    Serial.print(reference);
    Serial.print(" ");
    int read_adc = analogRead(A0);
    Serial.println(adc2lux(read_adc));
    //Serial.print(" ");
    //float simulated = controller.simulate(long(micros()),reference);
    //Serial.println(simulated);
    delayMicroseconds(100);
}
void FeedforwardControl(){
  float y = analogRead(A0);
  y = adc2lux(y);
  float error = reference - y;
  float u = staticalGain*error;
  if( u < 0) u = 0;
  if( u > 4095 ) u = 4095;
  int pwm = (int)u;
  analogWrite(LED_PIN, pwm);
  delay(10);
}

void calibrateStaticGain(){
  analogWrite(LED_PIN, 0);
  delay(100);
  float read_adc = analogRead(A0);
  float ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
  float ldrLux = calculateLDRLux(ldrResistor);
  externalIlluminance = ldrLux;
  analogWrite(LED_PIN, DAC_RANGE-1);
  delay(100);
  read_adc = analogRead(A0);
  ldrResistor = calculateLDRResistor(read_adc, DAC_RANGE);
  ldrLux = calculateLDRLux(ldrResistor);
  staticalGain = ldrLux-externalIlluminance;
}

float calculateLDRResistor(int analogInputValue, int dacRange){
  float quantizationInterval = VCC/dacRange;
  float luxVoltage = quantizationInterval*analogInputValue;
  float current = luxVoltage/resistor;
  float luxResistor = (VCC-luxVoltage)/current;
  return luxResistor;
}

float calculateLDRLux(float luxResistor){
  //b calculated assuming 10 LUX = 300Kohm
  float b = 6.2;
  float m = -0.86;
  float a = (log10(luxResistor)-b)/m;
  float lux = pow(10, a);
  return lux;
}
float adc2lux(int adc){
  float a = (3.3/4096*adc);
  float luxResistor = (3.3-a)*10000/a;
  float lux = pow(10, (log10(luxResistor)-6.2)/-0.86);
  return lux;
}
float lux2adc(float lux){
  return int(-0.0469 * pow(lux,2)+ 14.5 * lux + 30.4);
}
