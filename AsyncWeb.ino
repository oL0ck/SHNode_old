// this async server, so handles set at start and never changes till reboot
// but there is no lag for response(:
 
void AsyncWebInit(){
  // DEBUG
  server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request){
    String param;
    String pvalue;
    int args = request->args();
          for(int i=0;i<args;i++){
             param = request->argName(i).c_str();
             pvalue = request->arg(i).c_str();
             Serial.println("New: "+param+" Value: "+pvalue);
             };
    String senstatus;
    for (int i=0;i<=10;i++){
      if (sensorsona[i] == 1) {
        senstatus = senstatus+LEGEND[i]+" = ON <br>";
        }
      else {
        senstatus = senstatus+LEGEND[i]+" = OFF <br>";
        }
      };
    request->send(200, "text/html",
          "<!DOCTYPE HTML>"
          "<meta charset=\"utf-8\">"
          "<h1>Debug</h1>"
          "<h1>Sensors status:</h1>"
          ""+senstatus+""
          "Calibcount: "+String(mhz19calib,DEC)+""
          "<br><br><a href=\"/\">Main</a>  <a href=\"/debug\">Debug</a>  <a href=\"/config\">Configure Node</a>  <a href=\"/ReBoot\">Reboot</a>"
          "<br><title>"+NODENAME+":"+NODEIP+" \"</title>"
          );
  });

  // CONFIGURE
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    String param;
    String pvalue;
    int args = request->args();
          for(int i=0;i<args;i++){
             param = request->argName(i).c_str();
             pvalue = request->arg(i).c_str();
             Serial.println("New: "+param+" Value: "+pvalue);
             };
    if (pvalue.length()>=1 && pvalue.length()<=20) {
      NODENAME = pvalue;
      ConfigStoreName(NODENAME);
      };
    request->send(200, "text/html",
          "<!DOCTYPE HTML>"
          "<meta charset=\"utf-8\">"
          "<h1>Current SH-Node Name: "+NODENAME+"</h1>"
          "input new, and click apply"
          "<body>"
            "<form id=\"name\"></form>"
            "<p><input placeholder=\""+NODENAME+"\" name=\"name\" form=\"name\"></p>"
            "<p><input type=\"submit\" value=\"Apply\" form=\"name\"></p>"
          "</body>"
          "<a href=\"/Calibrate\">Calibrate MH-z19b</a><br>"
          "<a href=\"/CalibrateCCSBL\">Calibrate CCS811 baseline</a><br>"
          "<a href=\"/api?node_cfg_set=rebootcountreset\">Reset reboot counter</a><br>"
          "<br><a href=\"/\">Main</a>  <a href=\"/debug\">Debug</a>  <a href=\"/config\">Configure Node</a>  <a href=\"/ReBoot\">Reboot</a>"
          "<br><title>"+NODENAME+":"+NODEIP+" \"</title>"
          );  
  });
  
  // default-title page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Filter();
    request->send(200, "text/html",
          "<h1>MHZ19 - CO2 concentration: "+String(Fco2ppm,DEC)+"</h1>"
          "<h1>MHZ19 - temp: "+String(temp,DEC)+"</h1>"
          "<h1>Si7021 - Hummidity: "+String(Fsihummidity,2)+"</h1>"
          "<h1>Si7021 - Temp: "+String(Fsitemp,2)+"</h1>"
          "<h1>CCS811 - CO2: "+String(Fcssco2ppm)+"</h1>"
          "<h1>CCS811 - tVOC: "+String(Fcsstvoc)+"</h1>"
          "<h1>Firmware version: "+ver+"</h1>"
          "<h1>Sensors ack: "+String(sensorsona[0])+"</h1>"
          "<h1>Watchdog count: "+String(watchdog,DEC)+"("+String(WATCHDOGCOUNT,DEC)+")</h1>"
          "<h1>Current SH-Node Name: "+NODENAME+"</h1>"
          "<h1>Current reboot cycles: "+cyclesc+"</h1>"
          "<a href=\"/\">Main</a>  <a href=\"/debug\">Debug</a>  <a href=\"/config\">Configure Node</a>  <a href=\"/ReBoot\">Reboot</a>"
          "<meta http-equiv=\"Refresh\" content=\"5;URL=/\" />"
          "<br><title>"+NODENAME+":"+NODEIP+" \"</title>"
          );
  });

  // reboot if nessesary
  server.on("/ReBoot", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", 
          "<!DOCTYPE HTML>"
          "<meta charset=\"utf-8\">"
          "<h1>Rebooting......</h1>"
          "Auto refresh in 15 seconds....."
          "<meta http-equiv=\"Refresh\" content=\"15;URL=/\" />"
          "<br><title>"+NODENAME+":"+NODEIP+" \"</title>"
           );
    Serial.println("REBOOT request");
    ESP.restart();
  });

  // API
  server.on("/api", HTTP_GET, [](AsyncWebServerRequest *request){
    Filter();
    String param;
    String pvalue;
    String aresponse;
    int args = request->args();
          for(int i=0;i<args;i++){
             param = request->argName(i).c_str();
             pvalue = request->arg(i).c_str();
             //Serial.println("New api call: "+param+" Value: "+pvalue);
             //Serial.println(String(param.length())+" "+String(pvalue.length())); //4debug
             };
    if (pvalue.length()>=1 && pvalue.length()<=24  && param.length() >= 1 ) {
      if (param == "mhz19" && pvalue == "co2") aresponse = String(Fco2ppm,DEC);
      if (param == "mhz19" && pvalue == "temp") aresponse = String(temp,DEC);
      if (param == "mhz19" && pvalue == "co2_raw") aresponse = String(co2ppm,DEC);
      if (param == "si7021" && pvalue == "hummidity") aresponse = String(Fsihummidity,2);
      if (param == "si7021" && pvalue == "temp") aresponse = String(Fsitemp,2);
      if (param == "si7021" && pvalue == "hummidity_raw") aresponse = String(sihummidity,2);
      if (param == "si7021" && pvalue == "temp_raw") aresponse = String(sitemp,2);
      if (param == "ccs811" && pvalue == "co2") aresponse = String(Fcssco2ppm,DEC);
      if (param == "ccs811" && pvalue == "tvoc") aresponse = String(Fcsstvoc,DEC);
      if (param == "ccs811" && pvalue == "co2_raw") aresponse = String(cssco2ppm,DEC);
      if (param == "ccs811" && pvalue == "tvoc_raw") aresponse = String(csstvoc,DEC);
      if (param == "ccs811" && pvalue == "status") aresponse = ccsstatus;
      if (param == "ccs811" && pvalue == "intstatus") aresponse = ccsintstatus;
      if (param == "ccs811" && pvalue == "baseline") aresponse = GetBaselineCCS811();
      if (param.equalsIgnoreCase("ccs811_set_baseline")) {
        SetBaselineCCS811(pvalue.toInt());
        aresponse = pvalue+"<br> OK  <br> ";
        aresponse +=  "<br> Calibrating...."
          "<meta http-equiv=\"Refresh\" content=\"5;URL=/\" />";
      }
      if (param == "node_cfg" && pvalue == "ver") aresponse = ver;
      if (param == "node_cfg" && pvalue == "allsensorsstate") aresponse = sensorsona[0];
      if (param == "node_cfg" && pvalue == "watchdogcount") aresponse = String(watchdog,DEC);
      if (param == "node_cfg" && pvalue == "nodename") aresponse = NODENAME;
      if (param == "node_cfg" && pvalue == "rebootcount") aresponse = cyclesc;
      if (param == "node_cfg_set" && pvalue == "rebootcountreset") {
        ResetRebootCount();
        aresponse = "<meta http-equiv=\"Refresh\" content=\"1;URL=/\" />";
        };
      };
    request->send(200, "text/html", aresponse);
    Serial.println("API request: "+param+" Value: "+pvalue);
    Serial.println("Response: "+aresponse);
  });

  // MH-z19 calibrate ~30 mins
  server.on("/Calibrate", HTTP_GET, [](AsyncWebServerRequest *request){
     String percent = String(map(mhz19calib, 0, MHZ19CALIBCOUNTS, 0, 100));
     String a = "<meta http-equiv=\"Refresh\" content=\"5;URL=/Calibrate\" />"
                "<!DOCTYPE HTML>"
                "<meta charset=\"utf-8\">"
                "Calibrating....."+percent+"% done<br>"
                "Auto refresh every 5 seconds....."
                "<br><title>"+NODENAME+":"+NODEIP+" \"</title>";
     String b = "<!DOCTYPE HTML>"
                "<a href=\"/CalibrateStart\">CalibrateStart</a>"
                "</body><br>"
                "<a href=\"/\">Main</a>  <a href=\"/debug\">Debug</a>  <a href=\"/config\">Configure Node</a>  <a href=\"/ReBoot\">Reboot</a>"
                "<br><title>"+NODENAME+":"+NODEIP+" \"</title>";
     String c;
     if (mhz19calib > 0) {
       c = a;
       }
     else c = b;
     request->send(200, "text/html", 
          ""+c+""
           );
    Serial.println("Calibrate request");
  });
// MH-z19 calibrate PullTrigger
  server.on("/CalibrateStart", HTTP_GET, [](AsyncWebServerRequest *request){
     mhz19calib++;
     timermhz19calibrate.setTimeout(MHZ19CALIBTIMER);
     request->send(200, "text/html", 
          "Calibrating...."
          "<meta http-equiv=\"Refresh\" content=\"5;URL=/Calibrate\" />"
           );
    Serial.println("Calibrate request");
  });

// CalibrateCCSBL
  server.on("/CalibrateCCSBL", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", 
          "<h1>Calibrating CCS811 </h1><br>"
          "<h1>Current baseline: "+String(GetBaselineCCS811(),DEC)+"</h1>"
          "input new, and click apply"
          "<body>"
            "<form id=\"name\" action=\"/api?\" method=\"get\"></form>"
            "<p><input placeholder=\""+String(GetBaselineCCS811(),DEC)+"\" name=\"ccs811_set_baseline\" href=\"/config\" form=\"name\"></p>"
            "<p><input type=\"submit\" value=\"Apply\" form=\"name\"></p>"
          "</body>"
          "<br><br><a href=\"/\">Main</a>  <a href=\"/debug\">Debug</a>  <a href=\"/config\">Configure Node</a>  <a href=\"/ReBoot\">Reboot</a>"
          "<br><title>"+NODENAME+":"+NODEIP+" \"</title>"
           );
    Serial.println("Calibrate CCS811 config request");
  });
  
  // After all set starting server
  server.begin();
  }
