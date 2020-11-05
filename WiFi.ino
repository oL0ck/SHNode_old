// WiFi init
void WifiInit() {
  Serial.println();                                   // start WiFi
  Serial.println();
  Serial.print("Connecting to ");  
  Serial.println(ssid); 
  WiFi.mode(WIFI_STA);                                           // ONLY CLIENT mode
  WiFi.setAutoConnect(true);  
  WiFi.setAutoReconnect(true);                                   // turn automatically reconnects to hwAP in case it is disconnected
  delay(100);                                                    // fait for driver become ready
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(timerwifiinit.isExpired()){                              // wifi init watchdog
      delay(1000);
      Serial.println("Cannot connect wifi, restarting");
      ESP.restart();
      }
    }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");    
  Serial.println(WiFi.localIP());
  NODEIP = ipToString(WiFi.localIP());
  }

// check`s wifi by pinging gateway
void WIFiGWcheck() {
  if(timerwifi.isExpired()){          //  wifi check once a WIFICHECKTIMER with 1 ICMP packet
    Serial.print("For testing wifi we can always just ping the gateway: ");
      if (ping_start(WiFi.gatewayIP(), 1, 0, 0, 0)){
        Serial.println("OK");
        wifiok = 0;
        } else {
          Serial.println("FAILED");
          wifiok = wifiok + 1;
          watchdog = watchdog + 1;
          WiFi.reconnect();
          Serial.println("Warning watchdog is counting:"+String(watchdog,DEC));
          }
    timerwifi.restart();
    } 
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

