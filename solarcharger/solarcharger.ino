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
#include "config.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const char* ssid = STASSID;
const char* password = STAPSK;
ESP8266WebServer server(80);

int manualChargingButtonPin =2;
int showJSONButtonPin = 0;
int manualChargingPin = 13;

unsigned long lastTimestampJSON;
unsigned long lastTimestampCheck;
unsigned long lastManualChargingButtonPressed;
unsigned long startManualChargingTimestamp;
unsigned long lastButtonPressed;
int bat;
int jsonDisplayPosition = 0;
StaticJsonDocument<30000> jsonObject;
LinkedList<int> productionWList = LinkedList<int>();

bool manualChargingMode;
bool startedManualCharging;
int manualChargingDuration;

void setup() {
  Serial.begin(115200);
  pinMode(manualChargingPin,OUTPUT);
  pinMode(manualChargingButtonPin,INPUT);
  pinMode(showJSONButtonPin,INPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  manualChargingMode = false;
  startedManualCharging = false;
  manualChargingDuration = 0;
  lastTimestampJSON = millis();
  lastTimestampCheck = millis();
  lastButtonPressed = millis();
  setupWifi();
  getJSONFile();
}

void setupWifi() {
  showWifiConnecting();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display(); 
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/manualCharging", handleManualCharging);
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  //if button pressed for manual charging
  if (digitalRead(manualChargingButtonPin) == LOW) {
    if(millis()<=lastButtonPressed+displayTime*1000) {
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
    if(millis()<=lastButtonPressed+displayTime*1000) {
      showJSON();
    }
    delay(100);
    lastButtonPressed = millis();
  }

  // if in manual charging mode and last button pressed is older than 5 sec, start charging with selected time
  if(manualChargingMode && millis()> lastManualChargingButtonPressed + 5000 && startedManualCharging == false) {
    startCharging();
    startedManualCharging = true;
    startManualChargingTimestamp = millis();
  }

  // if in manual charging mode, check the duration of the progress and stop after selected time
  if(millis()>startManualChargingTimestamp+manualChargingDuration*1000*60*60) {
    manualChargingMode = false;
    startedManualCharging = false;
    stopCharging();
    
  }

  //get every timeMinNewJSON latest JSON file and store the production_w value in the list
  if(millis() > lastTimestampJSON + timeMinNewJSON*1000*60) {
    lastTimestampJSON = millis();
    getJSONFile();
  }

  // check every timeMinCheckCharging if charging is possible/neccessary, but only if not in manual charging mode
  if(millis() > lastTimestampCheck + 1000*60*timeMinCheckCharging && manualChargingMode==false) {
    lastTimestampCheck = millis();
    checkCharging();
  }


  //if in selecting manualCharging mode time 
  if (manualChargingMode && startedManualCharging == false) {
    showManualChargingMode();
  } else {
    //show default charging display
    if(millis()>lastButtonPressed+displayTime*1000) {
      standbyScreen();
    } else {
      updateDisplay();
    }
  }  
}



void getJSONFile() {
  //DeserializationError error = deserializeJson(jsonObject, httpGETRequest(SERVERURL));
  DeserializationError error = deserializeJson(jsonObject, "{\"Consumption_W\":496,\"FullChargeCapacity\":9765,\"GridFeedIn_W\":6419,\"Pac_total_W\":-3,\"Production_W\":6920,\"RSOC\":47,\"SetPoint_W\":0,\"Timestamp\":\"2021-06-1913:30:47\",\"USOC\":42,\"UTC_Offet\":2,\"ic_status\":{\"DCShutdownReason\":{\"CriticalBMSAlarm\":false,\"ErrorconditioninBMSinitialization\":false,\"HW_Shutdown\":false,\"HardWireOverVoltage\":false,\"HardWiredDrySignalA\":false,\"HardWiredUnderVoltage\":false,\"HoldingCircuitError\":false,\"InitializationTimeout\":false,\"InitializationofACcontactorfailed\":false,\"InitializationofBMShardwarefailed\":false,\"InitializationofDCcontactorfailed\":false,\"InitializationofInverterfailed\":false,\"InvalidornoSystemTypewasset\":false,\"InverterOverTemperature\":false,\"InverterUnderVoltage\":false,\"InverterVersionTooLowForDc-Module\":false,\"Manualshutdownbyuser\":false,\"MinimumrSOCofSystemreached\":false,\"NoSetpointreceivedbyHC\":false,\"ShutdownTimerstarted\":false,\"SystemValidationfailed\":false,\"VoltageMonitorChanged\":false},\"EclipseLed\":{\"PulsingGreen\":false,\"PulsingOrange\":false,\"PulsingWhite\":true,\"SolidRed\":false},\"FlatStatus\":{\"AutoMode\":true,\"Error\":false,\"FullChargePower\":false,\"FullDischargePower\":false,\"NotConnected\":true,\"Spare1\":false,\"Spare2\":false,\"Spare3\":false,\"Timeout\":false},\"MISCStatusBits\":{\"Dischargenotallowed\":false,\"F1open\":false,\"MinSystemSOC\":false,\"MinUserSOC\":false},\"MicrogridStatus\":{\"ContiniousPowerViolation\":false,\"DischargeCurrentLimitViolation\":false,\"LowTemperature\":false,\"MaxSystemSOC\":false,\"MaxUserSOC\":false,\"MicrogridEnabled\":false,\"MinSystemSOC\":false,\"MinUserSOC\":false,\"OverChargeCurrent\":false,\"OverDischargeCurrent\":false,\"PeakPowerViolation\":false,\"Protectisactivated\":false,\"TransitiontoOngridPending\":false},\"SetpointPriority\":{\"BMS\":false,\"EnergyManager\":true,\"Flat\":false,\"FullChargeRequest\":false,\"Inverter\":false,\"MinUserSOC\":false,\"TrickleCharge\":false},\"SystemValidation\":{\"CountryCodeSetstatusflag1\":false,\"CountryCodeSetstatusflag2\":false,\"SelftestErrorDCWiring\":false,\"SelftestPostponed\":false,\"SelftestPreconditionnotmet\":false,\"SelftestRunning\":false,\"Selftestsuccessfulfinished\":false},\"nrbatterymodules\":2,\"secondssincefullcharge\":71226,\"statebms\":\"ready\",\"statecorecontrolmodule\":\"ongrid\",\"stateinverter\":\"running\",\"timestamp\":\"SatJun1913:30:442021\"}}");
  productionWList.add(jsonObject["Production_W"]);
  if(productionWList.size()>15){
    productionWList.remove(0);
  }
  
  int fcc = jsonObject["FullChargeCapacity"];
  int usoc = jsonObject["USOC"];
  bat = constrain(((getAverageProduction() - defaultValues[getMonth(jsonObject["Timestamp"])][0] - p_auto_w)/ fcc * 100 + usoc ),0,100);
}


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  http.begin(serverName);
  http.addHeader("Auth-Token", AUTH_KEY);
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
