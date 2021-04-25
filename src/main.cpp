#include "utility.h"
#include <Wire.h>
#include "ArduinoJson.h"
#include "humiditySensor.h"
#include "webpage.h"
#include "DHT.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>   //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESPmDNS.h>

#define DHTPIN 4      // what pin we're connected to
#define DHTTYPE DHT22 // DHT 22  (AM2302)

// input pin for Soil Moisture sensor
const int analogSoilMoistureInPin = 34;

// set pin numbers
const int valveOutPin = 26; // the number of the valve pin
const int pumpOutPin = 5;   // the number of the pump pin

DHT dht(DHTPIN, DHTTYPE);

//AsyncWebServer server(80);
//const char *ssid = "QEIPI04 1692";
//const char *password = "tns-b!!T";

//const char *ssid = "Tanka";
//const char *password = "Pass#1024";

// TC201-1900AC_2.4G
//tc201@ait

// NPK sensor pins
#define RXD2 16
#define TXD2 17
//for modbus reading:
HardwareSerial mod(1);

int reading_soil_moisture = 0;
int reading_air_temp = 0;
int reading_air_humidity = 0;
int reading_N = 0;
int reading_P = 0;
int reading_K = 0;

const char *host = "littlefarm"; // set hostname. access as http://terracefarming.local

AsyncWebServer server(80);
DNSServer dns;

void setup()
{
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2) for reading NPK sensor;
  delay(10);

  mod.begin(9600, SERIAL_8N1, TXD2, RXD2);
  delay(10);

  delay(200);
   if (!MDNS.begin(host))  {
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println("mDNS responder started");
  Serial.printf("test serial");
  WiFi.mode(WIFI_STA);

  AsyncWiFiManager wm(&server, &dns);
  wm.resetSettings();
  bool res;
  res = wm.autoConnect("little_farm_AP", "password"); // password protected ap
  Serial.println("connected...to wifi :)");
  Serial.println(WiFi.localIP());

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("new JSON request received");
    char *response = generateMsgStr(reading_air_temp, reading_soil_moisture,
                                    reading_air_humidity, reading_N, reading_P, reading_K);
    Serial.println(response);

    request->send(200, "application/json", response);
  });
  server.begin();
  dht.begin();

  // initialize the valve pin as an output
  pinMode(valveOutPin, OUTPUT);
  pinMode(pumpOutPin, OUTPUT);
}
long lastupdate;
void loop()
{
  if (millis() > lastupdate + 5000)
  {
    lastupdate = millis();
    Serial.println(reading_soil_moisture);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    reading_air_humidity = dht.readHumidity();
    //reading_air_humidity = reading_soil_moisture + 45;
    // Read temperature as Celsius
    reading_air_temp = dht.readTemperature();
    // reading_air_temp =reading_soil_moisture + 60;
    Serial.print("Air temperature and Humidity ");
    Serial.print(reading_air_humidity);
    Serial.print(" ");
    Serial.println(reading_air_temp);

    // generating values for N, P, K parameters
    reading_N = nitrogen(mod);
    delay(250);
    reading_P = phosphorous(mod);
    delay(250);
    reading_K = potassium(mod);
    delay(250);

    printNPK(reading_N, reading_P, reading_K);
  }
}