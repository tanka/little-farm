// you can adjust the threshold value
int thresholdDry = 2600;
int thresholdVeryWet = 800; // 800-2600 is good value

// returns 0 for dry, 1 good moisture, 2 for too high moisture
float humidty(int analogSoilMoistureInPin) {
// read the input on analog pin 35:
  int sensorValue = analogRead(analogSoilMoistureInPin);
  Serial.print(sensorValue);
  float moisture_val = sensorValue*100/4095;
  
  if(sensorValue < thresholdVeryWet){
    Serial.println(" - Doesn't need watering");
    return moisture_val;
  }
  else if(sensorValue > thresholdDry) {
    Serial.println(" - Time to water your plant");
    return moisture_val;
  }
  else {
    Serial.println(" - Good Moisture");
    return moisture_val;
    }
}