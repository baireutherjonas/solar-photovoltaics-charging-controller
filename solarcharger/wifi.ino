IPAddress createAP(String ssid, String password) {
  Serial.println("create AP");
  WiFi.disconnect();
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  return myIP;
}

bool connectWiFi(String ssid, String password) {
  WiFi.begin(ssid, password);
  int connectionTries = 0;
  while (WiFi.status() != WL_CONNECTED && connectionTries < 40) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display(); 
    connectionTries++;
  }

  if(connectionTries >= 40) {
    return false;
  } else {
    return true;
  }
}
