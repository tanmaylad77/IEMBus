#include <Wire.h>

#define current_adc_addr 0x50
#define voltage_adc_addr 0x51

void setupADCs() {
  Wire.begin();
  Wire.beginTransmission(current_adc_addr);
  Wire.write(0x02);
  Wire.write(0x20);
  Wire.endTransmission();
  Wire.beginTransmission(voltage_adc_addr);
  Wire.write(0x02);
  Wire.write(0x20);
  Wire.endTransmission();
}

// returns integer adc result
int readADC(uint8_t address) {
  unsigned int data[2];
  Wire.beginTransmission(address);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  if(Wire.available() == 2) {
	  data[0] = Wire.read();
	  data[1] = Wire.read();
  }
  int raw_adc = (((data[0] & 0x0F) * 256) + (data[1] & 0xF0)) / 16;
  return raw_adc;
}

// get current - MUST TUNE THIS
float getCurrent(float ref_5v = 4.52) {
  return  50.0 * ((ref_5v / 256) * readADC(current_adc_addr)) / 20.0;
}

// get voltage - MUST TUNE THIS
float getVoltage(float ref_5v = 4.52) {
  return 17 * ((ref_5v / 256) * readADC(voltage_adc_addr));
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  setupADCs();
}

void loop()
{
  Serial.print("Voltage: ");
  Serial.print(getVoltage());
  Serial.print("V and Current: ");
  Serial.print(getCurrent());
  Serial.println("A");
  delay(500);
}
