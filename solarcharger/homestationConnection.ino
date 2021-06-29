void getJSONFile(String serverURL, String authKey) {
  DeserializationError error = deserializeJson(jsonRequestFile, httpGETRequest(serverURL, authKey));
  productionWList.add(jsonRequestFile["Production_W"]);
  if(productionWList.size()>15){
    productionWList.remove(0);
  }
  
  bat = constrain(((getAverageProduction() - jsonConfigFile["defaultValues"][getMonth(jsonRequestFile["Timestamp"])][0].as<int>() - jsonConfigFile["local"]["p_auto_w"].as<int>())/ jsonRequestFile["FullChargeCapacity"].as<int>() * 100 + jsonRequestFile["USOC"].as<int>() ),0,100);
}


String httpGETRequest(String serverURL, String authKey) {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverURL);
  http.addHeader("Auth-Token", authKey);
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
