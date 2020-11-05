// sensors init
void SensorsInit() {
  myHTU21D.begin();                                  // SI7021 Temp and Hummidity sensor
  CCS811Core::status returnCode = mySensor.begin();  // CCS811 CO2 sensor init
  mySensor.setDriveMode(1);                          // CCS811 drive mode 1, auto measure every 1 secs
  CCSStatusGet(returnCode);                          // Catch CCS init code
  if (NODENAME.indexOf("SDLo")>-1) {
    mhz19.begin(14, 12);    }
  else {
    mhz19.begin(rx_pin, tx_pin);// MH-z19b sensor init
    };
  mhz19.setAutoCalibration(true);                   // Tutrn off auto calibration
  analogReadResolution(12);                          // Tur ADC to 12 bits for Emon
  }

// sensors ask
void SensorsAsk() {
  if(timer.isExpired() && sensorsona[0] && mhz19calib == 0) {            //  requesing sensorts.... every SCANNSENSORS
      sscount = sscount+1;
      if (sscount == 1 && sensorsona[sscount]) co2ppm = mhz19.getPPM();
      if (sscount == 2 && sensorsona[sscount]) temp = mhz19.getTemperature();
      if (sscount == 3 && sensorsona[sscount]) sihummidity = myHTU21D.readHumidity();
      if (sscount == 4 && sensorsona[sscount]) sitemp = myHTU21D.readTemperature(SI70xx_TEMP_READ_AFTER_RH_MEASURMENT);
      if (sscount == 5 && sensorsona[sscount]) mySensor.setEnvironmentalData(Fsihummidity,Fsitemp);
      if (sscount == 6 && sensorsona[sscount]) mySensor.readAlgorithmResults();
      if (sscount == 7 && sensorsona[sscount] && mySensor.dataAvailable()) cssco2ppm = mySensor.getCO2();
      if (sscount == 8 && sensorsona[sscount] && mySensor.dataAvailable()) csstvoc = mySensor.getTVOC();
      //if (sscount == 9 && sensorsona[sscount]) emon.serialprint(); 
      if (sscount == 10 && sensorsona[sscount]) {
        Serial.println("Sensors Idling");
        Serial.println("Si7021 Hummidity: "+String(sihummidity,2)+" Temp: "+String(sitemp,2));
        Serial.print("MH-z19b ");Serial.println(" CO2:"+String(co2ppm,DEC));
        Serial.print("CSS811 compensated CO2:");Serial.print(cssco2ppm);Serial.print(" tVOC:");Serial.println(csstvoc);  

        }
      timer.restart();
      CCSIntStatusGet();
      };//  end of sensors and watchdog
  
  // calibrate mgz19
  if (mhz19calib > 0 && timermhz19calibrate.isExpired()) { // calibrating MH-z19
       Serial.print("co2: "); Serial.print(mhz19.getPPM()); Serial.println("ppm now.");
       if (mhz19calib == MHZ19CALIBCOUNTS/2 || mhz19calib == (MHZ19CALIBCOUNTS/8)*4 || mhz19calib == (MHZ19CALIBCOUNTS/8)*6) mhz19.calibrateZero();
       mhz19calib++;
       if (mhz19calib >= MHZ19CALIBCOUNTS) {
         mhz19calib = 0;
         timermhz19calibrate.setTimeout(MHZ19CALIBTIMER*10);
         };
       timermhz19calibrate.restart();
      }
  if (sscount >= 10 ) sscount = 0;
  }

void CCSStatusGet( CCS811Core::status errorCode )
{
  switch ( errorCode )
  {
    case CCS811Core::SENSOR_SUCCESS:
      Serial.print("SUCCESS");
      ccsstatus = "SUCCESS";
      break;
    case CCS811Core::SENSOR_ID_ERROR:
      Serial.print("ID_ERROR");
      ccsstatus = "ID_ERROR";
      break;
    case CCS811Core::SENSOR_I2C_ERROR:
      Serial.print("I2C_ERROR");
      ccsstatus = "I2C_ERROR";
      break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
      Serial.print("INTERNAL_ERROR");
      ccsstatus = "INTERNAL_ERROR";
      break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
      Serial.print("GENERIC_ERROR");
      ccsstatus = "GENERIC_ERROR";
      break;
    default:
      Serial.print("Unspecified error.");
  }
}

void CCSIntStatusGet()
{
  ccsintstatus = "OK";
  uint8_t error = mySensor.getErrorRegister();

  if ( error == 0xFF ) //comm error
  {
    Serial.println("Failed to get ERROR_ID register.");
    ccsintstatus = "Failed to get ERROR_ID register.";
  }
  else
  {
    if (error & 1 << 5) {
      Serial.print("HeaterSupply");
      ccsintstatus = "HeaterSupply";
    }
    if (error & 1 << 4) {
      Serial.print("HeaterFault");
      ccsintstatus = "HeaterFault";
    }
    if (error & 1 << 3) {
      Serial.print("MaxResistance");
      ccsintstatus = "MaxResistance";
    }
    if (error & 1 << 2) {
      Serial.print("MeasModeInvalid");
      ccsintstatus = "MeasModeInvalid";
    }
    if (error & 1 << 1) {
      Serial.print("ReadRegInvalid");
      ccsintstatus = "ReadRegInvalid";
    }
    if (error & 1 << 0) {
      Serial.print("MsgInvalid");
      ccsintstatus = "MsgInvalid";
    }
  }
  
}

void SetBaselineCCS811(unsigned int newbaseline) {
  mySensor.setBaseline(newbaseline);
  CCSIntStatusGet();
}

unsigned int GetBaselineCCS811() {
  unsigned int check = mySensor.getBaseline();
  if ( check>0 ) return check;
}

