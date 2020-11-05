

void ConfigInit(){
  Serial.println("Getting config parameters from NVS...");  
  Config.begin("Node-SH", false);
    // reboot counter
    cyclesc = Config.getUInt("cyclesc", 0);  // Reboot times counter, return 0 if none
    cyclesc++;
    Serial.printf("Current reboot cycles: %u\n", cyclesc);
    Config.putUInt("cyclesc", cyclesc);
    // nodename
    NODENAME = Config.getString("NodeName","SH-Node");
    Serial.printf("Current current NodeName: %s\n", NODENAME);
    // set prifile parametes
    if (NODENAME == "SH-Node" || NODENAME.indexOf("Test")>-1) {
      Serial.println("Using \"Test\" or DEFAULT profile");
      sensorsona[0] = 0;  // disable all, but serial debug
      sensorsona[1] = 0;
      sensorsona[2] = 0;
      sensorsona[3] = 0;
      sensorsona[4] = 0;
      sensorsona[5] = 0;
      sensorsona[6] = 0;
      sensorsona[7] = 0;
      sensorsona[8] = 0;
      sensorsona[9] = 0;
      sensorsona[10] = 1;
      }
    else if (NODENAME == "Kuhnya") {   
      Serial.println("Using \"Kuhnya\" profile");
      sensorsona[0] = 1;  // enable global and mh-z19 and serial debug
      sensorsona[1] = 1;
      sensorsona[2] = 1;
      sensorsona[3] = 0;
      sensorsona[4] = 0;
      sensorsona[5] = 0;
      sensorsona[6] = 0;
      sensorsona[7] = 0;
      sensorsona[8] = 0;
      sensorsona[9] = 0;
      sensorsona[10] = 1; 
      }
    else if (NODENAME.indexOf("Lolin")>-1) {   
      Serial.println("Using \"Lolin\"-like wire pins profile");
      Wire.begin(17,22);  // this for lolin lite board, alternate pins
      sensorsona[0] = 1;  // enable global and mh-z19 and serial debug
      sensorsona[1] = 1;
      sensorsona[2] = 1;
      sensorsona[3] = 1;
      sensorsona[4] = 1;
      sensorsona[5] = 1;
      sensorsona[6] = 1;
      sensorsona[7] = 1;
      sensorsona[8] = 1;
      sensorsona[9] = 0;
      sensorsona[10] = 1; 
      }
    else if (NODENAME.indexOf("Emon")>-1) {   
      Serial.println("Using \"Emon\" with lolin-like wire pins profile");
      //Wire.begin(17,22);  // this for lolin lite board, alternate pins
      sensorsona[0] = 1;  // enable global and mh-z19 and serial debug
      sensorsona[1] = 0;
      sensorsona[2] = 0;
      sensorsona[3] = 0;
      sensorsona[4] = 0;
      sensorsona[5] = 0;
      sensorsona[6] = 0;
      sensorsona[7] = 0;
      sensorsona[8] = 0;
      sensorsona[9] = 1;
      sensorsona[10] = 1; 
      }
    else if (NODENAME.indexOf("Debug")>-1) {   
      Serial.println("Using \"DEBUG\" - lolin");
      //Wire.begin(17,22);  // this for lolin lite board, alternate pins
      sensorsona[0] = 1;  // enable global and mh-z19 and serial debug
      sensorsona[1] = 0;
      sensorsona[2] = 0;
      sensorsona[3] = 0;
      sensorsona[4] = 0;
      sensorsona[5] = 0;
      sensorsona[6] = 0;
      sensorsona[7] = 0;
      sensorsona[8] = 0;
      sensorsona[9] = 1;
      sensorsona[10] = 1; 
      }
    else if (NODENAME.indexOf("SDLo")>-1) {   
      Serial.println("Using \"SDLo\" - lolin");
      Wire.begin(17,22);  // this for lolin lite board, alternate pins
      sensorsona[0] = 1;  // enable global and mh-z19 and serial debug
      sensorsona[1] = 1;
      sensorsona[2] = 1;
      sensorsona[3] = 1;
      sensorsona[4] = 1;
      sensorsona[5] = 1;
      sensorsona[6] = 1;
      sensorsona[7] = 1;
      sensorsona[8] = 1;
      sensorsona[9] = 1;
      sensorsona[10] = 1; 
      }
    else {
      Serial.println("Using DEFAULT all ON profile");
      sensorsona[0] = 1;  // enable All if profile called, but not configured
      sensorsona[1] = 1;
      sensorsona[2] = 1;
      sensorsona[3] = 1;
      sensorsona[4] = 1;
      sensorsona[5] = 1;
      sensorsona[6] = 1;
      sensorsona[7] = 1;
      sensorsona[8] = 1;
      sensorsona[9] = 0;
      sensorsona[10] = 1; 
      }
  Config.end();
  ArduinoOTA.setHostname(NODENAME.c_str()); // Using logical Node Name
  };

void ResetRebootCount() {
  Config.begin("Node-SH", false);
  Config.putUInt("cyclesc", 0);
  Config.end();
  cyclesc = 0;
  }

void ConfigStoreName(String NewName){
  Config.begin("Node-SH", false);
    // put node name to ROM
    Config.putString("NodeName",NewName);
  Config.end();  
  };

//void SetSensGlobal(int[])
