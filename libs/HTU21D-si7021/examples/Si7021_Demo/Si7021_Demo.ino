/***************************************************************************************************/
/* 
  This is an example for Si7021 Digital Humidity & Temperature Sensor

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This sensor uses I2C bus to communicate, specials pins are required to interface

  Connect chip to pins:    SDA        SCL
  Uno, Mini, Pro:          A4         A5
  Mega2560, Due:           20         21
  Leonardo:                2          3
  ATtiny85:                0(5)       2/A1(7)   (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                                 & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-01:          GPIO0/D5   GPIO2/D3  (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             GPIO4/D2   GPIO5/D1
  WeMos D1 Mini:           GPIO4/D2   GPIO5/D1

  GNU GPL license, all text above must be included in any redistribution, see link below for details
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <Wire.h>
#include <HTU21D.h>

/*
HTU21D(resolution)

resolution:
HTU21D_RES_RH12_TEMP14 - RH: 12Bit, Temperature: 14Bit, by default
HTU21D_RES_RH8_TEMP12  - RH: 8Bit,  Temperature: 12Bit
HTU21D_RES_RH10_TEMP13 - RH: 10Bit, Temperature: 13Bit
HTU21D_RES_RH11_TEMP11 - RH: 11Bit, Temperature: 11Bit
*/
HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);


void setup()
{
  Serial.begin(115200);
  
  while (myHTU21D.begin() != true)
  {
    Serial.println("Si7021 sensor is faild or not connected");
    delay(5000);
  }
  Serial.println("Si7021 sensor is active");
}

void loop()
{
  /* DEMO - 1 */
  Serial.println(F("DEMO 1: 12 Bit Resolution")); 
  Serial.print(F("Compensated Humidity: ")); Serial.print(myHTU21D.readHumidity()); Serial.println(F(" +-2%"));

  Serial.println(F("DEMO 1: 14 Bit Resolution")); 
  Serial.print(F("Temperature.........: ")); Serial.print(myHTU21D.readTemperature(SI70xx_TEMP_READ_AFTER_RH_MEASURMENT)); Serial.println(F(" +-0.5C"));


  /* DEMO - 2 */
  Serial.println(F("DEMO 2: 11 Bit Resolution"));
  myHTU21D.setResolution(HTU21D_RES_RH11_TEMP11);

  Serial.print(F("Compensated Humidity: ")); Serial.print(myHTU21D.readHumidity()); Serial.println(F(" +-2%"));

  Serial.println(F("DEMO 2: 11 Bit Resolution"));
  Serial.print(F("Temperature.........: ")); Serial.print(myHTU21D.readTemperature(SI70xx_TEMP_READ_AFTER_RH_MEASURMENT)); Serial.println(F(" +-0.5C"));


  /* DEMO - 3 */
  Serial.println(F("DEMO 3: Battery Status"));
  if   (myHTU21D.batteryStatus() == true) Serial.println(F("Battery.........: OK.  Level > 1.90v"));
  else                                    Serial.println(F("Battery.........: LOW. Level < 1.90v"));


  /* DEMO - 4 */
  Serial.println(F("DEMO 4:"));
  Serial.print(F("Firmware version....: ")); Serial.println(myHTU21D.readFirmwareVersion());


  /* DEMO - 5 */
  Serial.println(F("DEMO 5:"));
  Serial.print(F("Sensor's ID.........: ")); Serial.println(myHTU21D.readDeviceID());


  /* back to lib. default resolution */
  myHTU21D.setResolution(HTU21D_RES_RH12_TEMP14);


  /* DEMO - END */
  Serial.print(F("DEMO starts over again in 20 sec."));
  delay(20000);
}
