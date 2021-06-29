void startServer() {
  server.on("/", handleRoot);
  server.on("/manualCharging", handleManualCharging);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void handleManualCharging() {
  for (int i = 0; i < server.args(); i++) {
    if(server.argName(i)=="duration") {
      manualChargingDuration = server.arg(i).toInt();
      startCharging(manualChargingPin);
      startedManualCharging = true;
      manualChargingMode = true;
      startManualChargingTimestamp = millis();
    }
  }
  handleRoot();
}

void handleRoot() {
  String usoc = jsonRequestFile["USOC"];
  String prodW = jsonRequestFile["Production_W"];
  String result = "<!DOCTYPE html> <html><head><meta charset=\"UTF-8\"></head><body>";
  result += "PV " + prodW +"W<br>";
  result += "B ist " + usoc + "% <br>";
  result += "B 1h "+ String(bat) + "%<br>";
  result += "B min "+ jsonConfigFile["defaultValues"][getMonth(jsonRequestFile["Timestamp"])][1].as<String>() + "%<br>";
  if(manualChargingMode){
    result += "Lädt noch " + remainingChargingDuration();
  } else {
    result += getCountdown() + "<br><br>";
  }
  result += "<a href=\"/manualCharging?duration=0\">Stop charging</a>";
  result += "<a href=\"/manualCharging?duration=1\">Start charging 1h</a>";
  result += "<a href=\"/manualCharging?duration=2\">Start charging 2h</a>";
  result += "<a href=\"/manualCharging?duration=3\">Start charging 3h</a>";
  result += "<a href=\"/manualCharging?duration=4\">Start charging 4h</a>";
  result += "<a href=\"/manualCharging?duration=5\">Start charging 5h</a>";
  result += "</body></html>";
  server.send(200, "text/html", result);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
