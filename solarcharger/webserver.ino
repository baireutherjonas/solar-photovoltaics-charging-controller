void handleManualCharging() {
  for (int i = 0; i < server.args(); i++) {
    if(server.argName(i)=="duration") {
      manualChargingDuration = server.arg(i).toInt();
      startCharging();
      startedManualCharging = true;
      manualChargingMode = true;
      startManualChargingTimestamp = millis();
    }
  }
  handleRoot();
}

void handleRoot() {
  String usoc = jsonObject["USOC"];
  String prodW = jsonObject["Production_W"];
  String result = "<!DOCTYPE html> <html><head><meta charset=\"UTF-8\"></head><body>";
  result += "PV " + prodW +"W<br>";
  result += "B ist " + usoc + "% <br>";
  result += "B 1h "+ String(bat) + "%<br>";
  result += "B min "+ String(defaultValues[getMonth(jsonObject["Timestamp"])][0]) + "%<br>";
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
