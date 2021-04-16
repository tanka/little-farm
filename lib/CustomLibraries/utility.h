#include <Arduino.h>
char buff[200];

char msg[100];

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

byte nitrogen(HardwareSerial mod){
  // digitalWrite(DE,HIGH);
  // digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(nitro,sizeof(nitro))==8){
//    digitalWrite(DE,LOW);
//    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(mod.read(),HEX);
    values[i] = mod.read();
   // Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte phosphorous(HardwareSerial mod){
  delay(10);
  if(mod.write(phos,sizeof(phos))==8){
    for(byte i=0;i<7;i++){
    values[i] = mod.read();
  //  Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(HardwareSerial mod){
  delay(10);
  if(mod.write(pota,sizeof(pota))==8){
    for(byte i=0;i<7;i++){
    values[i] = mod.read();
  //  Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
