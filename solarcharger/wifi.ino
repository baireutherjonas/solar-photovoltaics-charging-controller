IPAddress createAP(String ssid, String password) {
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  return myIP;
}

void connectWiFi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
