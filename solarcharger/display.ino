void showManualChargingMode() {
  
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Manuelles Laden");
  display.setCursor(0, 10);
  display.println(String(manualChargingDuration) + " Stunden");
  display.setCursor(0, 30);
  display.println(String(5- (millis() - lastManualChargingButtonPressed)/1000) +  " Sekunden Warten zum Starten");
  display.setCursor(0, 440);
  display.println("Nochmal Button drücken zum Zeit ändern");
  display.display(); 
}

void showWifiConnecting() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Mit WLAN verbinden");
  display.setCursor(0, 10);
  display.println("SSID: ");
  display.println(STASSID);
  display.display(); 
}

void updateDisplay() {
  
  display.clearDisplay();

  String usoc = jsonObject["USOC"];
  String prodW = jsonObject["Production_W"];
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("PV " + prodW +"W");
  display.setCursor(0, 10);
  display.println("B ist " + usoc + "% ");
  display.setCursor(0, 20);
  display.println("B 1h "+ String(bat) + "%");
  display.setCursor(0, 30);
  display.println("B min "+ String(defaultValues[getMonth(jsonObject["Timestamp"])][0]) + "%");
  display.setCursor(0, 40);
  if(manualChargingMode){
    display.println("Lädt noch " + remainingChargingDuration());
  } else {
    display.println(getCountdown());
  }
  display.display(); 
}

void showJSON() {
  int scrolling = 0;
  JsonObject documentRoot = jsonObject.as<JsonObject>();
  for(int j=0;j<140;j++) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    jsonDisplayPosition = 0;
    for (JsonPair keyValue : documentRoot) {
      showJSONHelper(keyValue,scrolling);
    }
    display.display(); 
    scrolling = scrolling + 10;
    delay(300);
  }
}

void showJSONHelper(JsonPair keyValue, int scrolling) {
    display.setCursor(0, jsonDisplayPosition*10-scrolling);
    JsonVariant value = keyValue.value();
    if(value.is<JsonObject>()) {
      JsonObject checker = value.as<JsonObject>();
      display.println(String(keyValue.key().c_str()) + ":");
      jsonDisplayPosition++;
      for (JsonPair keyValue : checker) {
        showJSONHelper(keyValue,scrolling);
      }
    } else {
      String displayText = String(keyValue.key().c_str()) + ":" + String(keyValue.value().as<String>());
      display.println(displayText);
      jsonDisplayPosition=jsonDisplayPosition+int(displayText.length()/22)+1;
    }
}

void standbyScreen() {
    display.clearDisplay();
    display.display();   
}
