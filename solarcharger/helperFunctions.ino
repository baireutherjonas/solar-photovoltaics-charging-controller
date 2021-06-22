
String remainingChargingDuration() {
  int remainSec = (1000*60*60*manualChargingDuration-(millis() - startManualChargingTimestamp));
  remainSec = remainSec / 1000;
  int remainHour = remainSec / 60/60;
  int remainMin = remainSec /60 % 60;
  remainSec = remainSec % 60;
  return String(remainHour)+":"+ String(remainMin)+":"+String(remainSec);
}

String getCountdown() {
  int remainSec = (1000*60*timeMinCheckCharging-(millis() - lastTimestampCheck));
  remainSec = remainSec / 1000;
  int remainMin = remainSec / 60;
  remainSec = remainSec % 60;
  return String(remainMin)+":"+String(remainSec);
}

int getAverageProduction() {
  int calcsum = 0;
  for (int i = 0; i <= productionWList.size(); i++) {
    calcsum += productionWList.get(i);
  }
  Serial.print("Average: ");
  Serial.print(calcsum/productionWList.size());
  Serial.print(" ");
  Serial.println(productionWList.size());
  return calcsum / productionWList.size();
}

int getMonth(String timestamp)  {
  timestamp = timestamp.substring(timestamp.indexOf("-")+1);
  return timestamp.substring(0, timestamp.indexOf("-")+1).toInt();
}

void checkCharging() {
  if(bat>defaultValues[getMonth(jsonObject["Timestamp"])][1]) {
    startCharging();
  } else {
    stopCharging();
  }  
}
