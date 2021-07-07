#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LinkedList.h>

#include <SPI.h>
#include <SD.h>

unsigned long lastTimestampJSON;
unsigned long lastTimestampCheck;
unsigned long lastManualChargingButtonPressed;
unsigned long startManualChargingTimestamp;
unsigned long lastButtonPressed;
bool manualChargingMode;
bool normalrun;
bool startedManualCharging;
bool config_file_available;
bool sd_connected;
int manualChargingDuration;
int bat;

int manualChargingButtonPin =2;
int showJSONButtonPin = 0;
int manualChargingPin = 16;
const int chipSelect = 15;

ESP8266WebServer server(80);

DynamicJsonDocument jsonConfigFile(4096);
StaticJsonDocument<3000> jsonRequestFile;
LinkedList<int> productionWList = LinkedList<int>();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  
  pinMode(manualChargingPin,OUTPUT);
  pinMode(manualChargingButtonPin,INPUT);
  pinMode(showJSONButtonPin,INPUT);

  normalrun = false;
  config_file_available = false;
  manualChargingMode = false;
  startedManualCharging = false;
  manualChargingDuration = 0;
  lastTimestampJSON = millis();
  lastTimestampCheck = millis();
  lastButtonPressed = millis();
  
  configDisplay();
  startServer();

  checkIfConfigFileAvailable();

  if(config_file_available) {
    loadConfig();
  } else {
    firstStartHint();
  }
}

void checkIfConfigFileAvailable() {
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    sd_connected = false;
  } else {
    sd_connected = true;
  }

  if(sd_connected) {
    if(SD.exists("config.json")) {
      File myFile = SD.open("config.json", FILE_READ);
      DeserializationError err = deserializeJson(jsonConfigFile, myFile);
      if (err) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(err.c_str());
      }

      config_file_available = true;
      myFile.close();
    }
  }
  
}

void firstStartHint() {
    IPAddress ipAddr = createAP("Charger Station","123456789");
    showConfigurationHint("Charger Station", ipAddr);
}


void loadConfig() {
  showWifiConnecting(jsonConfigFile["wifi"]["ssid"].as<String>());
  if(connectWiFi(jsonConfigFile["wifi"]["ssid"].as<String>(),jsonConfigFile["wifi"]["pw"].as<String>())) {
    getJSONFile(jsonConfigFile["server"]["url"].as<String>(),jsonConfigFile["server"]["authkey"].as<String>());
    normalrun = true;
  } else {
    firstStartHint();
  }
  
}

void loop() {
  server.handleClient();
  if(normalrun) {
    handleNormal();
  }

  if(SD.begin(chipSelect) && !sd_connected){
    checkIfConfigFileAvailable();
      loadConfig();
  }

  if (!SD.begin(chipSelect)) {
    sd_connected = false;
  }
}

void handleNormal() {  
  //if button pressed for manual charging
  if (digitalRead(manualChargingButtonPin) == LOW) {
    if(millis()<=lastButtonPressed+jsonConfigFile["local"]["displayTime"].as<int>()*1000) {
      //turn into manual charging mode
      manualChargingMode = true;
      // change charging duration on button pressed
      if( manualChargingDuration < 5) {
        manualChargingDuration++;
      } else {
        manualChargingDuration = 0;
      }
  
      // store last button pressed, for starting after 5 seconds nothing is pressed
      lastManualChargingButtonPressed = millis();
    }
    delay(100);
    lastButtonPressed = millis();
  }

  // if button pressed for showing JSON
  if (digitalRead(showJSONButtonPin) == LOW) {
    if(millis()<=lastButtonPressed+jsonConfigFile["local"]["displayTime"].as<int>()*1000) {
      showJSON();
    }
    delay(100);
    lastButtonPressed = millis();
  }

  // if in manual charging mode and last button pressed is older than 5 sec, start charging with selected time
  if(manualChargingMode && millis()> lastManualChargingButtonPressed + 5000 && startedManualCharging == false) {
    startCharging(manualChargingPin);
    startedManualCharging = true;
    startManualChargingTimestamp = millis();
  }

  // if in manual charging mode, check the duration of the progress and stop after selected time
  if(millis()>startManualChargingTimestamp+manualChargingDuration*1000*60*60 && manualChargingMode) {
    manualChargingMode = false;
    startedManualCharging = false;
    stopCharging(manualChargingPin);
    
  }

  //get every timeMinNewJSON latest JSON file and store the production_w value in the list
  if(millis() > lastTimestampJSON + jsonConfigFile["local"]["timeMinNewJSON"].as<int>()*1000*60) {
    lastTimestampJSON = millis();
    getJSONFile(jsonConfigFile["server"]["url"].as<String>(),jsonConfigFile["server"]["authkey"].as<String>());
  }

  // check every timeMinCheckCharging if charging is possible/neccessary, but only if not in manual charging mode
  if(millis() > lastTimestampCheck + 1000*60*jsonConfigFile["local"]["timeMinCheckCharging"].as<int>() && manualChargingMode==false) {
    lastTimestampCheck = millis();
    checkCharging();
  }


  //if in selecting manualCharging mode time 
  if (manualChargingMode && startedManualCharging == false) {
    showManualChargingMode(manualChargingDuration,  lastManualChargingButtonPressed);
  } else {
    //show default charging display
    if(millis()>lastButtonPressed+jsonConfigFile["local"]["displayTime"].as<int>()*1000) {
      standbyScreen();
    } else {
      updateDisplay(jsonRequestFile["Production_W"].as<String>(),jsonRequestFile["USOC"].as<String>(),String(bat));
    }
  }  
}
