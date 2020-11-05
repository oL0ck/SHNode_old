// WactDog section, restart esp if something

void WatchDog(){
  // check wifi  
  WIFiGWcheck();
  
  // sensors control
  if (timerwatchdogsensors.isExpired() && sensorsona[0]) {
        //if ( co2ppm >= 10001 || co2ppm <= -1 || sihummidity == 255 || sitemp == 255 ) {
        if ( co2ppm >= 10001 || co2ppm <= -1 || sihummidity == 255 || sitemp == 255 ) {
        watchdog = watchdog + 1;
        Serial.println("Warning watchdog is counting:"+String(watchdog,DEC));        
        } else {
          if(wifiok == 0)
            watchdog = 0;
          else
            Serial.println("Wifi gateway does not response "+String(wifiok,DEC)+" minute(s)");
          }
    timerwatchdogsensors.restart();
    }
  
  // Glitch detected - rebooting
  if (watchdog >= WATCHDOGCOUNT) {
    Serial.println("Watchdog glitch found, software restart in 10 seconds");
    delay(10000);
    ESP.restart();
    };
}

