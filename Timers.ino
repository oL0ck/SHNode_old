void TimersInit() {
 timerwatchdogsensors.setTimeout(SENSORSWDCHECK);   // arm sensors watchdog timer
 timerwatchdogsensors.restart();
 
 timer.setTimeout(SCANSENSORS);                     // arm main timers
 timer.restart();
 
 timerwifi.setTimeout(WIFICHECKTIMER);              // arm wifi watchdog timer
 timerwifi.restart();
 
 timercominit.setTimeout(COMUNTILREADY);            // arm com-usb timer
 timercominit.restart();

 timerwifiinit.setTimeout(WIFIINITTIMER);           // arm wifi init timer
 timerwifiinit.restart();
 
 timermhz19calibrate.setTimeout(MHZ19CALIBTIMER);    // arm calibration timer
 timermhz19calibrate.restart();
 }

