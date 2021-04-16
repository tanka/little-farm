#include "utility.h"
#include <Wire.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "humiditySensor.h"
//#include "actuators.h"

#include <SoftwareSerial.h>
#include "DHT.h"

#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// input pin for Soil Moisture sensor
const int analogSoilMoistureInPin = 34;

// set pin numbers
const int valveOutPin =  26;    // the number of the valve pin
const int pumpOutPin =  5;    // the number of the pump pin

DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);
//const char *ssid = "QEIPI04 1692";
//const char *password = "tns-b!!T";

const char *ssid = "Tanka";
const char *password = "Pass#1024";

// NPK sensor pins
#define RXD2 16
#define TXD2 17
//for modbus reading: 
HardwareSerial mod(1);

int reading_soil_moisture = 0;
int reading_air_temp = 0;
int reading_air_humidity= 0;
int reading_N= 0;
int reading_P= 0;
int reading_K= 0;

//SoftwareSerial mod(2,3);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void setup()
{
  Serial.begin(115200);
    //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2) for reading NPK sensor;
    delay(10);

  mod.begin(9600, SERIAL_8N1, TXD2, RXD2);
      delay(10);
 
  delay(200);
  Serial.printf("test serial");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
      delay(5000);
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("new JSON request received");
    char* response = generateMsgStr(reading_air_temp, reading_soil_moisture, 
      reading_air_humidity, reading_N, reading_P, reading_K);
    Serial.println(response);
    
    request->send(200, "application/json", response);
  });
  server.on("/get-message", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<100> data;
    if (request->hasParam("message"))
    {
      data["message"] = request->getParam("message")->value();
    }
    else
    {
      data["message"] = "No message parameter";
    }
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);
  });
  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/post-message",
   [](AsyncWebServerRequest *request, JsonVariant &json) {
    StaticJsonDocument<200> data;
    if (json.is<JsonArray>())
    {
      data = json.as<JsonArray>();
    }
    else if (json.is<JsonObject>())
    {
      data = json.as<JsonObject>();
    }
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);
    Serial.println(response);
  });
  server.addHandler(handler);
  server.onNotFound(notFound);
  server.begin();

  dht.begin();

   // initialize the valve pin as an output
  pinMode(valveOutPin, OUTPUT);
  pinMode(pumpOutPin, OUTPUT);
}

void loop() {
  delay(5000);
  // print soil humidity values
//  reading_soil_moisture = humidty(analogIn);
  
 // reading_soil_moisture = rand()%100;
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
  Serial.print( " " );
  Serial.println(reading_air_temp);

    // generating values for N, P, K parameters
  reading_N = nitrogen(mod);
  delay(250);
  reading_P = phosphorous(mod);
  delay(250);
  reading_K = potassium(mod);
  delay(250);

  printNPK(reading_N, reading_P, reading_K);

  delay(5000);
}