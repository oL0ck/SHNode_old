/***************************************************************************************************/
/* 
  This is an Arduino code calculates Dew Point, Humidex and Winchill temperature by using
  DSHT21, HTU21D or Si70xx Digital Humidity & Temperature Sensor.

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
#include <Wire.h>              //https://github.com/enjoyneering/ESP8266-I2C-Driver
#include <HTU21D.h>
#include <LiquidCrystal_I2C.h> //https://github.com/enjoyneering/LiquidCrystal_I2C
#include <ESP8266WiFi.h>

#define LCD_ROWS      4        //qnt. of lcd rows
#define LCD_COLUMNS   20       //qnt. of lcd columns

#define DEGREE_SYMBOL 0xDF     //degree symbol from lcd ROM
#define SPACE_SYMBOL  0x20     //space  symbol from lcd ROM

#define WIND_SPEED    5        //assuming wind speed is 5 m/sec
#define ERROR         255      //error code

uint8_t temperature_icon[8] = {0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x1F, 0x1F, 0x0E};
uint8_t humidity_icon[8]    = {0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00};

float temperature = 0;
float humidity    = 0;
float dewpoint    = 0;
float humidex     = 0;
float windchill   = 0;

/*
HTU21D(resolution)

resolution:
HTU21D_RES_RH12_TEMP14 - RH: 12Bit, Temperature: 14Bit, by default
HTU21D_RES_RH8_TEMP12  - RH: 8Bit,  Temperature: 12Bit
HTU21D_RES_RH10_TEMP13 - RH: 10Bit, Temperature: 13Bit
HTU21D_RES_RH11_TEMP11 - RH: 11Bit, Temperature: 11Bit
*/ 
HTU21D            myHTU21D(HTU21D_RES_RH12_TEMP14);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


/**************************************************************************/
/*
    setup()

    Main setup
*/
/**************************************************************************/
void setup()
{
  WiFi.enableAP(false);                                         //disable soft AP
  WiFi.enableSTA(false);                                        //disable station/client

  Serial.begin(115200);

  /* LCD connection check */  
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS, LCD_5x8DOTS) != true) //20x4 display, LCD_5x8DOTS pixels size, SDA - D2, SCL - D1
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);
  }
  lcd.print("PCF8574 is OK");
  delay(1000);
  lcd.clear();

  /* HTU21D connection check */
  while (myHTU21D.begin() != true)
  {
    lcd.setCursor(0, 0);
    lcd.print("HTU21D error");
    delay(5000);
  }
  lcd.clear();

  lcd.print("HTU21D OK");
  delay(1000);
  lcd.clear();

  /* load custom symbol to CGRAM */
  lcd.createChar(0, temperature_icon);
  lcd.createChar(1, humidity_icon);

  /* prints static text */
  lcd.setCursor(0, 0);                                                  //set 1-st colum & 1-st row, first colum & row started at zero
  lcd.write((uint8_t)0);                                                //print custom tempereture symbol

  lcd.setCursor(10, 0);
  lcd.write((uint8_t)1);                                                //print custom humidity symbol

  lcd.setCursor(0, 1);
  lcd.print("Dewpoint :");

  lcd.setCursor(0, 2);
  lcd.print("Humidex  :");

  lcd.setCursor(0, 3);
  lcd.print("Windchill:");
}

/**************************************************************************/
/*
    loop()

    Main loop
*/
/**************************************************************************/
void loop()
{
  humidity    = myHTU21D.readCompensatedHumidity();
  temperature = myHTU21D.readTemperature();

  dewpoint    = calculateDewPoint(temperature, humidity);
  humidex     = calculateHumidex(temperature, dewpoint);
  windchill   = calculateWindchill(temperature, WIND_SPEED);

  /* prints dynamic temperature data */
  lcd.setCursor(1, 0);
  if   (temperature != HTU21D_ERROR) lcd.print(temperature);
  else                               lcd.print("xxx");
  lcd.write(DEGREE_SYMBOL);
  lcd.write(SPACE_SYMBOL);

  /* prints dynamic humidity data */
  lcd.setCursor(11, 0);
  if   (humidity != HTU21D_ERROR) lcd.print(humidity);
  else                            lcd.print("xxx");
  lcd.print("%");
  lcd.write(SPACE_SYMBOL);

  /* prints dynamic dewpoint data */
  lcd.setCursor(10, 1);
  if   (dewpoint != ERROR) lcd.print(dewpoint);
  else                     lcd.print("xxx");
  lcd.write(DEGREE_SYMBOL);
  lcd.write(SPACE_SYMBOL);

  /* prints dynamic humidex data */
  lcd.setCursor(10, 2);
  if   (humidex != ERROR) lcd.print(humidex);
  else                    lcd.print("xxx");
  lcd.write(DEGREE_SYMBOL);
  lcd.write(SPACE_SYMBOL);

  /* prints dynamic windchill data */
  lcd.setCursor(10, 3);
  if   (windchill != ERROR) lcd.print(windchill);
  else                      lcd.print("xxx");
  lcd.write(DEGREE_SYMBOL);
  lcd.write(SPACE_SYMBOL);

  /* prints serial data */
  Serial.print("Humidity............: "); Serial.print(humidity);    Serial.println("%");
  Serial.print("Temperature.........: "); Serial.print(temperature); Serial.println("C");
  Serial.print("Dew Point...........: "); Serial.print(dewpoint);    Serial.println("C");
  Serial.print("Humidex/Feels like..: "); Serial.print(humidex);     Serial.print("C, "); humidexDiscomfortIndex(humidex);
  Serial.print("Windchill/Feels like: "); Serial.print(windchill);   Serial.print("C, "); windchillDiscomfortIndex(humidex);

  delay(20000);
}

/**************************************************************************/
/*
    calculateDewPoint()

    Dew point calculation
*/
/**************************************************************************/
float calculateDewPoint(float temperature, float humidity)
{
  float a = 17.271;
  float b = 237.7;

  if (temperature != HTU21D_ERROR && humidity != HTU21D_ERROR)
  {
    float gamma    = ((a * temperature) / (b + temperature)) + log(humidity / 100);
    float dewpoint = (b * gamma) / (a - gamma);

    return (dewpoint);
  }
  return ERROR;
}

/**************************************************************************/
/*
    calculateHumidex()

    Humidex calculation

    Humidex is an index number used to describe how hot the weather feels 
    to the average person, by combining the effect of heat and humidity.

    Humidex is calculated as:
       Humidex = air temperature + h
       h = (0.5555) * (e - 10.0)
       e = 6.11 * exp(5417.7530 * ((1 / 273.16) - (1 / dewpoint)))
*/
/**************************************************************************/
float calculateHumidex(float temperature, float dewpoint)
{
  if (temperature > 5 && temperature != HTU21D_ERROR && dewpoint != ERROR)
  {
    float e       = 5417.7530 * ((1 / 273.16) - (1 / (273.16 + dewpoint)));
    float humidex = temperature + 0.5555 * ( 6.11 * exp(e) - 10);

    return (humidex);
  }
  return ERROR;
}

/**************************************************************************/
/*
    calculateWindchill()

    Windchill calculation

    Wind chill is an index number used to describe how cold the weather feels
    to the average person, by combining the effect of cold and wind speeds.

    Wind chill is calculated:
       Windchill = 13.12 + 0.6215 * ta - 11.37 * v^0.16 + 0.3965 * ta * v^0.16
       ta = air temperature below 10C
       v  = wind velocity greater than 4.8 km/h
*/
/**************************************************************************/
float calculateWindchill(float temperature, float velocity)
{
  if (temperature != HTU21D_ERROR && temperature <= 5 && velocity >= 5)
  {
    float windchill = 13.12 + 0.6215 * temperature - 11.37 * pow(velocity, 0.16) + 0.3965 * temperature * pow(velocity, 0.16);

    return (windchill);
  }
  return ERROR;
}

/**************************************************************************/
/*
    humidexDiscomfortIndex()

    Describes discomfort levels of humidex index
*/
/**************************************************************************/
void humidexDiscomfortIndex(float humidex)
{
  if ((humidex >= 21 ) && (humidex < 27))
  {
    Serial.println("Little discomfort.");
  }
  else if ((humidex >= 27) && (humidex < 35))
  {
    Serial.println("Noticeable discomfort.");
  }
  else if ((humidex >= 35) && (humidex < 40))
  {
    Serial.println("Great discomfort.");
  }
  else if ((humidex >= 40) && (humidex < 46))
  {
    Serial.println("Intense discomfort.");
  }
  else if ((humidex >= 46) && (humidex < 54))
  {
    Serial.println("Dangerous discomfort.");
  }
  else
  {
    Serial.println("Heat stroke warning.");
  }
}

/**************************************************************************/
/*
    windchillDiscomfortIndex()

    Describes discomfort levels of windchill index
*/
/**************************************************************************/

void windchillDiscomfortIndex(float windchill)
{
  if (windchill >= -24)
  {
    Serial.println("Low risk of frostbite.");
  }
  else if ((windchill < -24) && (windchill >= -39))
  {
    Serial.println("High risk of frostbite within 30 minutes of exposure.");
  }
  else if ((windchill < -39) && (windchill >= -47))
  {
    Serial.println("High risk of frostbite within 5 to 10 minutes of exposure.");
  }
  else if ((windchill < -47) && (windchill >= -54))
  {
    Serial.println("High risk of frostbite within 2 to 5 minutes of exposure.");
  }
  else
  {
    Serial.println("High risk of frostbite within 2 minutes of exposure.");
  }
}
