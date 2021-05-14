#include <Arduino.h>
char buff[200];

char msg[100];

//-------Certificates----------------//
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n"\
"MIID3TCCAsWgAwIBAgIUEZQh/Dh3e4uHzeyZ0SR9AWGJouswDQYJKoZIhvcNAQEL\n" \
"BQAwfjELMAkGA1UEBhMCVEgxDDAKBgNVBAgMA1BUSDEMMAoGA1UEBwwDQUlUMQww\n" \
"CgYDVQQKDANBSVQxDDAKBgNVBAsMA0FJVDEMMAoGA1UEAwwDQUlUMSkwJwYJKoZI\n" \
"hvcNAQkBFhpzaGFybWEudGFua2FuYXRoQGdtYWlsLmNvbTAeFw0yMTA0MjMwODI1\n" \
"MjVaFw0zMTA0MjEwODI1MjVaMH4xCzAJBgNVBAYTAlRIMQwwCgYDVQQIDANQVEgx\n" \
"DDAKBgNVBAcMA0FJVDEMMAoGA1UECgwDQUlUMQwwCgYDVQQLDANBSVQxDDAKBgNV\n" \
"BAMMA0FJVDEpMCcGCSqGSIb3DQEJARYac2hhcm1hLnRhbmthbmF0aEBnbWFpbC5j\n" \
"b20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCwOtewcht3C1cz4MY5\n" \
"/YDtteA95sysBODLkD9gznvhsuJCSjxytQ/PygiUBtzkA0hN/7+bsF2X2Wxrp8UJ\n" \
"mc+w4IoNHKBhuu50duIEo5bpqgkQGVZnM/VgunZAh/LmJd6sPk4ZcE+TwcvFemXI\n" \
"nab+348urkb5fWA2kuu2wqBvtjIaM98PwXhSzgbm+p9uHiDbb///RR3xbGre2N1h\n" \
"8OMCpTh9kWamx8EV9JvcDb60F9TeKGYtp/L2v9YNHb4MrRVedUw6aBSWxsBVWMiU\n" \
"UK5b52VBX8c72lRJqR5kGXMyC2I645ri/NNiSUr7mEnM98+aT0qdkkKOc3iOKfXg\n" \
"6kqdAgMBAAGjUzBRMB0GA1UdDgQWBBQOJ4LnbU+EOgBParEIs/9z2EVgdDAfBgNV\n" \
"HSMEGDAWgBQOJ4LnbU+EOgBParEIs/9z2EVgdDAPBgNVHRMBAf8EBTADAQH/MA0G\n" \
"CSqGSIb3DQEBCwUAA4IBAQBHT8e3b3oCzDiUdu5ECv9wvuDSFS8UI6E6JOfB2JFn\n" \
"h5G1lY78/pPzcyxjaLM6QGNd255626vbYfpe6IHwoBtP+CUijfhOZvApEKcLxYwX\n" \
"ETJ1vw6uK1T3Lz7VdBiOJq77BIFkkxGp8fZNiaxcyf7MMajIYDGFELk5sI7Ia7Gy\n" \
"xtgRjpFhw2LROkz2TKDBXeO7ADqW9Ssy6JCV4krODz/CyqjqXMBjhFY/zadUt661\n" \
"8QbrTVGevUqVZWvVyCmfLXaxTjvOgNC7DjBaQpSZBCgDu8l0+TBVlDoAAga02YlQ\n" \
"vC/oDVnUq0vKIDSsZ0ORdNk8KVrfjb+e5ls/4rpXuwxF\n" \
"-----END CERTIFICATE-----\n";


const char* device001_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEowIBAAKCAQEAyHfQFKCBBWoVWq7HxcriTteyWThk8f1emt9D1ydJzx6phOdU\n" \
"y/YtZZZTiZi5sqWWUn7Mm37i6cgIyjFi7f5PPzEYDekM2aG0P0db6w13dxyOUS7h\n" \
"lww8RZL0eXSN1Q5xMFmQf8iwhCMmx3KO3mEAorAQtGgi9AFt3Vh9qISsNrPkkIVg\n" \
"Hcy+rN8lhd73te1Nj1BPXkTq/AxQolXy0CwtBTEHS5LOMhqFgI/ryDcBYn6Pv7zC\n" \
"Cw5G96zw68NDhUX0Q2J+xgVIHO/TzFRb9yDxp8USzZ+8zYWgHUik7D4Khu61q5bv\n" \
"B/pTd2hGVLezTj9IIHRTMdQsqRpptDjJeFGNAwIDAQABAoIBAQCGl7Kz9qNJYFUg\n" \
"AlMMYFAbQUR0FKHpuwQmlRn2PWGX1u1X1v0ex5TpLMm/izO6VuSbVJjJGaL99Ri5\n" \
"r/jM8z5/ndWZmUvQXnonNiDnqq334ctHhGNfx4475BOR7AhB8KVcIfYbZUHmJbem\n" \
"lyqG8be8gttkVLqK+t6piv0rBJCfVG4LOc63rjNjjTD+mDA0yPD48reM1q/x6m7f\n" \
"UWX49sct6OMG4mWfcXk4PZ0zMnEr8DOVJ9lGA0Sw1Kp/oIljxz2dNErcgVSr7fVP\n" \
"xKNHpgCInM0YWfSKHkUYUu4IKZggzjf5avjsgqSTlkEiFsDKKBov8Xl53fQh0anu\n" \
"armcr/+xAoGBAOsoXARZ7Vg18Nkv5Jn2BPbJTASYVG06PfqZCy3KBAymPwIpWSfc\n" \
"JDVVH89XNht1IsXwPFysO3llAf0TpZ+HxD1MoOzV4oWrRSwQaKjHdW3XbBzRUzfq\n" \
"oMoedalYZ3dHun6Zv1LbECnsI2nAg9Dxm5hoygpuTwS3SJYjSnAJHqMbAoGBANo8\n" \
"W7tBAft6V7lQos5D2rD0M8auyzjZstLhpvF7xSMnWoG7/+WXLpNerDKFourD/XOR\n" \
"SWDE5eHTMZYJv9YJCM66YIBYbdQtCIK1p6tRi/3cf9ywEg3cN0PKSXoULMYSMAz4\n" \
"73rCc+X10N/no9X5zBRAyXR0j5Z6LDGeySv+NXQ5AoGAXlJ27ivf3oTsUUE7kWGz\n" \
"xgUh36n2G0XEPHMMn9Yq36QDG+vX3fwX7ehx9OmHQ2Yth3V/0LAovt17+bXate1w\n" \
"EiJdXgHAdVsoSK9gO2qduGwy6jAwD/qleczT5xQVlCuKK2Oh8Sr7BcW4wp88gB+Z\n" \
"CH3npF1whnS5wbbwEZYm2scCgYAZLK1IRI99+vbzoaGmOZ2YnBL88FzxCeve97jS\n" \
"UnzDOV7vaEh0q6PrzBgBG/lXK5SpKDFWwF/kVQB7VlSS1Cf38PgA1jKVLzy0F1Bo\n" \
"KtgDSNRXGNh8UNLvNv6S3pDrqF1yoaS+fH/yHxEhQw9bN52bGH4kuO13ZlCCyNZc\n" \
"BM8VwQKBgFCwwqYrwh8i7kGmqA4EkvvLNqt6ZWnyxaTrEjGhNa4LS4Xy3wqEMXh1\n" \
"MMlbZOPzIek7heOwsea6LW5shbMk29OqBFQXNTA/yDekPPTw1GwRGuokf54A9BPR\n" \
"api3aRpfACqpRXtDj+KrfhHygAiNV0T/8xlAq9wIQgLwwqzSBkBP\n" \
"-----END RSA PRIVATE KEY-----\n";



const char* device001_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDkDCCAngCFBM1b0Y+DFlSCuUE4YH6kijdDKMUMA0GCSqGSIb3DQEBCwUAMH4x\n" \
"CzAJBgNVBAYTAlRIMQwwCgYDVQQIDANQVEgxDDAKBgNVBAcMA0FJVDEMMAoGA1UE\n" \
"CgwDQUlUMQwwCgYDVQQLDANBSVQxDDAKBgNVBAMMA0FJVDEpMCcGCSqGSIb3DQEJ\n" \
"ARYac2hhcm1hLnRhbmthbmF0aEBnbWFpbC5jb20wHhcNMjEwNDIzMDkwODMyWhcN\n" \
"MzEwNDIxMDkwODMyWjCBijELMAkGA1UEBhMCVEgxDDAKBgNVBAgMA1BUSDEMMAoG\n" \
"A1UEBwwDQUlUMQwwCgYDVQQKDANBSVQxDDAKBgNVBAsMA0FJVDEYMBYGA1UEAwwP\n" \
"TVFUVCBDTElFTlQgMDAxMSkwJwYJKoZIhvcNAQkBFhpzaGFybWEudGFua2FuYXRo\n" \
"QGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMh30BSg\n" \
"gQVqFVqux8XK4k7Xslk4ZPH9XprfQ9cnSc8eqYTnVMv2LWWWU4mYubKlllJ+zJt+\n" \
"4unICMoxYu3+Tz8xGA3pDNmhtD9HW+sNd3ccjlEu4ZcMPEWS9Hl0jdUOcTBZkH/I\n" \
"sIQjJsdyjt5hAKKwELRoIvQBbd1YfaiErDaz5JCFYB3MvqzfJYXe97XtTY9QT15E\n" \
"6vwMUKJV8tAsLQUxB0uSzjIahYCP68g3AWJ+j7+8wgsORves8OvDQ4VF9ENifsYF\n" \
"SBzv08xUW/cg8afFEs2fvM2FoB1IpOw+CobutauW7wf6U3doRlS3s04/SCB0UzHU\n" \
"LKkaabQ4yXhRjQMCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAa1ydJRSkgoygdZTc\n" \
"j3OicqoxAKgMRsRAfIVJmRY60rfld2xuNvQrYnX1PpZORGPd7M5Np6/3Lg9yhlhr\n" \
"1e3diazmIny6nItRCPDyJGMS2Zwe3d7xBRpsMCVyqO29iuBcBCCUwWAg0oFzJf3b\n" \
"ijSfA8gf9ZEeKizgTSYrQbC+LoVar5JCHU5FdNw+uEcNYVuMVJms5hoxQAM1V9dT\n" \
"5+Kwlm8qd9D6qJhvrh5s40xq2qsmK2EYvQcqdHsFkXLTt6X9LewCoIQJhvJlb8N8\n" \
"pHoEtVvxKtg9gsu2LOu/y0QhJzen5j05MLyQPJvQlxy3I7kUD4zngiQI7lGJHjtG\n" \
"ubjICDAMMAoGCCsGAQUFBwMC\n" \
"-----END CERTIFICATE-----\n";
// end of certificates ---------------//

char* generateMsgStr(int reading_air_temp, int reading_soil_moisture, 
    int reading_air_humidity, int reading_N, int reading_P, int reading_K){
  msg[0]='\0';
  strcat(msg, "{\"air_temp\":");
  itoa(reading_air_temp, buff, 10 );
  strcat(msg,buff);

  strcat(msg,", \"air_hum\":");
  itoa(reading_air_humidity, buff, 10 );
  strcat(msg,buff);  

  strcat(msg,", \"soil_mois\":");
  itoa(reading_soil_moisture, buff, 10 );
  strcat(msg,buff);

  strcat(msg,", \"soil_N\":");
  itoa(reading_N, buff, 10 );
  strcat(msg,buff);  

  strcat(msg,", \"soil_K\":");
  itoa(reading_K, buff, 10 );
  strcat(msg,buff);  

  strcat(msg,", \"soil_P\":");
  itoa(reading_P, buff, 10 );
  strcat(msg,buff);  

  strcat(msg,"}");
  return msg;
}
void printNPK(int reading_N, int reading_P, int reading_K){
  Serial.print("Nitrogen: ");
  Serial.print(reading_N);
  Serial.println(" mg/kg");
  Serial.print("Phosphorous: ");
  Serial.print(reading_P);
  Serial.println(" mg/kg");
  Serial.print("Potassium: ");
  Serial.print(reading_K);
  Serial.println(" mg/kg ");
}

byte values[11];

//const byte code[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD};// {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};

const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte nitrogen(HardwareSerial *mod){
  // digitalWrite(DE,HIGH);
  // digitalWrite(RE,HIGH);
  delay(10);
  if(mod->write(nitro,sizeof(nitro))==8){
//    digitalWrite(DE,LOW);
//    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod->read();
 //     Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte phosphorous(HardwareSerial *mod){
  delay(10);
  if(mod->write(phos,sizeof(phos))==8){
    for(byte i=0;i<7;i++){
    values[i] = mod->read();
  //  Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(HardwareSerial *mod){
  delay(10);
  if(mod->write(pota,sizeof(pota))==8){
    for(byte i=0;i<7;i++){
    values[i] = mod->read();
  //  Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}

// you can adjust the threshold value
int thresholdDry = 2600;
int thresholdVeryWet = 800; // 800-2600 is good value

// returns 0 for dry, 1 good moisture, 2 for too high moisture
float soilMoisture(int analogSoilMoistureInPin) {
// read the input on analog pin 34:
  int sensorValue = analogRead(analogSoilMoistureInPin);
  Serial.print(sensorValue);
  float moisture_val = sensorValue*100/4095;
  
  if(sensorValue < thresholdVeryWet){
    Serial.println(" - Doesn't need watering");
    return 100 - moisture_val;
  }
  else if(sensorValue > thresholdDry) {
    Serial.println(" - Time to water your plant");
    return 100 - moisture_val;
  }
  else {
    Serial.println(" - Good Moisture");
    return 100 - moisture_val;
    }
}