
#include "htmlpages.h"

void startServer() {
  server.on("/", handleRoot);
  server.on("/manualCharging", handleManualCharging);
  server.on("/config", handleConfig);
  server.on("/uploadConfig", saveConfig);
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

void saveConfig() {
  for (int i = 0; i < server.args(); i++) {
    if(server.argName(i)=="configfile") {
      String configfileString= server.arg(i);
      DeserializationError err = deserializeJson(jsonConfigFile, configfileString);
      if(SD.exists("config.json")) {
        SD.remove("config.json");
      }
      File myFile = SD.open("config.json", FILE_WRITE);
      myFile.println(configfileString);
      myFile.close();
      loadConfig();
    }
  }
  handleRoot();
}

void handleRoot() {
  String usoc = jsonRequestFile["USOC"];
  String prodW = jsonRequestFile["Production_W"];
  String result = html_prefix;
  String home_prefix = html_home_prefix;
  result += home_prefix;
  result += "<li class=\"list-group-item\">PV " + prodW +"W</li>";
  result += "<li class=\"list-group-item\">B ist " + usoc + "%</li>";
  result += "<li class=\"list-group-item\">B 1h "+ String(bat) + "%</li>";
  result += "<li class=\"list-group-item\">B min "+ jsonConfigFile["defaultValues"][getMonth(jsonRequestFile["Timestamp"].as<String>())]["battery_min"].as<String>() + "%</li>";
  if(manualChargingMode){
    result += "<li class=\"list-group-item\">Remaining charging duration " + remainingChargingDuration() + "</li>";
  } else {
    result += "<li class=\"list-group-item\">Next data request: " + getCountdown()+"</li>";
  }
  
  String body_home = html_home_body;
  String body = html_body;
  server.send(200, "text/html", result+body_home+body);
}

void handleConfig() {
  String output;
  serializeJson(jsonConfigFile, output);
  String result = html_prefix;
  result += "<form action=\"uploadConfig\" method=\"post\"><div class=\"mb-3\">";
  result += "<textarea name=\"configfile\" cols=\"100\" rows=\"30\">";
  result += output;
  result += "</textarea>";
  result += "<input type=\"submit\" value=\"save\" /></div></form><p><a class=\"btnbtn-primary\" data-bs-toggle=\"collapse\" href=\"#collapseExample\" role=\"button\" aria-expanded=\"false\" aria-controls=\"collapseExample\">Default config</a></p><div class=\"collapse\" id=\"collapseExample\"><div class=\"card card-body\"> {\"wifi\":{\"ssid\":\"WiFi Name\",\"pw\":\"WiFi Password\"},\"server\":{\"url\":\"sonnenbatterie-url\",\"authkey\":\"sonnenbatterie-api key\"},\"local\":{\"p_auto_w\":2400,\"timeMinNewJSON\":1,\"timeMinCheckCharging\":15,\"displayTime\":600},\"defaultValues\":[{\"basicDemand\":1050,\"battery_min\":95},{\"basicDemand\":1060,\"battery_min\":95},{\"basicDemand\":720,\"battery_min\":95},{\"basicDemand\":550,\"battery_min\":66},{\"basicDemand\":360,\"battery_min\":36},{\"basicDemand\":280,\"battery_min\":25},{\"basicDemand\":280,\"battery_min\":24},{\"basicDemand\":270,\"battery_min\":26},{\"basicDemand\":320,\"battery_min\":37},{\"basicDemand\":510,\"battery_min\":66},{\"basicDemand\":770,\"battery_min\":95},{\"basicDemand\":960,\"battery_min\":95}]}</div></div>";
  
  String body = html_body;
  server.send(200, "text/html", result+body);
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
