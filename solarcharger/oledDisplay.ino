#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int jsonDisplayPosition;

void configDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }  
}

void showManualChargingMode(int manualChargingDuration, long lastManualChargingButtonPressed) {
  
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

void showWifiConnecting(String ssid) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Mit WLAN verbinden");
  display.setCursor(0, 10);
  display.println("SSID: ");
  display.println(ssid);
  display.display(); 
}

void showConfigurationHint(String ssid, IPAddress ipaddr) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Mit WLAN verbinden");
  display.setCursor(0, 10);
  display.println("SSID: ");
  display.println("   " + ssid);
  display.print("http://");
  display.print(ipaddr);
  display.println(" aufrufen");
  display.println("");
  display.println("Alternativ SD-Karte einlegen");
  display.display(); 
}

void updateDisplay(String prodW, String usoc, String bat) {
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("PV " + prodW +"W");
  display.setCursor(0, 10);
  display.println("B ist " + usoc + "% ");
  display.setCursor(0, 20);
  display.println("B 1h "+ bat + "%");
  display.setCursor(0, 30);
  display.println("B min "+ jsonConfigFile["defaultValues"][getMonth(jsonRequestFile["Timestamp"])][1].as<String>() + "%");
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
  JsonObject documentRoot = jsonRequestFile.as<JsonObject>();
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
