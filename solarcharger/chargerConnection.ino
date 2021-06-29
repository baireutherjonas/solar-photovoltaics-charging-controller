
void startCharging(int manualChargingPin) {
    digitalWrite(manualChargingPin,HIGH);
}

void stopCharging(int manualChargingPin) {
    digitalWrite(manualChargingPin,LOW);
}
