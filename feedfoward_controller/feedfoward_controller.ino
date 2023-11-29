#include <math.h>

const int LED_PIN = 15;
const float DAC_RANGE = 4096.0;
const float samp_time {10}; //10ms
float reference {10.0};
float staticalGain;
float externalIlluminance;
String inString = ""; // string to hold input

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); //default is 10 
  analogWriteFreq(60000); //60KHz, about max
  analogWriteRange(DAC_RANGE); //100% duty cycle
  calibrateStaticGain();  
}

void loop() { 
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inString += (char)inChar;
    }
    if (inChar == '\n') { //got newline, now process input
      reference = float(inString.toInt()); // print value
      inString = "";
      
    }
  }
  int read_adc = analogRead(A0);
  float y = adc2lux(read_adc);
  Serial.print(reference);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.print(100);
  Serial.print(" ");
  Serial.println(y);
  delay(samp_time);  
  FeedforwardControl();
}
void FeedforwardControl(){
  float dc = reference/staticalGain;
  float u = dc * DAC_RANGE;
  int pwm = int(u);
  analogWrite(LED_PIN, pwm);
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
