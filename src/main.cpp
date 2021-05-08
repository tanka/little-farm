#include "utility.h"
#include <Wire.h>
#include "ArduinoJson.h"
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

#include <WiFiClientSecure.h> //TLS protocol
#include <PubSubClient.h>
#include <Wire.h>

#define DHTPIN 4      // what pin we're connected to
#define DHTTYPE DHT22 // DHT 22  (AM2302)

// input pin for Soil Moisture sensor
const int analogSoilMoistureInPin = 34;

// set pin numbers
const int valvePin = 26;     // the number of the valve pin
const int pumpPin = 5;       // the number of the pump pin
const int solarVoltPin = 35; // the number of the valve pin

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

// deep sleep implementation prameters
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 600      /* Time ESP32 will go to sleep (in seconds) */
long sleepTime;

RTC_DATA_ATTR int bootCount = 0;
// deep sleep -- end

// MQTT PART //
// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char *mqtt_server = "192.168.1.174";
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;
bool sleepEnable = true;

void callback(String topic, byte *message, unsigned int length);

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if (topic == "pump/full" && messageTemp == "on")
  {
    sleepEnable = false;
    Serial.print("Opening valve to release 1L of water... ");
    Serial.println("activating pump for 30s");
    digitalWrite(pumpPin, HIGH);
    Serial.println("water flowing");
    long timeDelay = millis();  
    while (millis()<timeDelay +60000)
    {digitalWrite(pumpPin, HIGH);}

    sleepEnable = true;
    Serial.println("deactivating pump");
    digitalWrite(pumpPin, LOW);
    Serial.println("water NOT flowing");
  }
  if (topic == "pump/half" && messageTemp == "on")
  {
    sleepEnable = false;
    Serial.print("Opening valve to release 0.5L of water... ");
    Serial.println("activating pump for 15s");
    digitalWrite(pumpPin, HIGH);
    Serial.println("water flowing");
    long timeDelay = millis();  
    while (millis()<timeDelay +20000)
    {digitalWrite(pumpPin, HIGH);}
    sleepEnable = true;
    Serial.println("deactivating pump");
    digitalWrite(pumpPin, LOW);
    Serial.println("water NOT flowing");
  }
  if (topic == "valve/10" && messageTemp == "on")
  {
    sleepEnable = false;
    Serial.print("Opening valve to release 10mL of NPK... ");
    Serial.println("opening valve for 10s");
    digitalWrite(valvePin, HIGH);
    Serial.println("NPK flowing");
    long timeDelay = millis();  
    while (millis()<timeDelay +15000)
    {digitalWrite(valvePin, HIGH);}
    sleepEnable = true;
    Serial.println("closing valve");
    digitalWrite(valvePin, LOW);
    Serial.println("NPK NOT flowing");
  }
  if (topic == "valve/5" && messageTemp == "on")
  {
    sleepEnable = false;
    Serial.print("Opening valve to release 5mL of NPK... ");
    Serial.println("opening valve for 5s");
    digitalWrite(valvePin, HIGH);
    Serial.println("NPK flowing");
    long timeDelay = millis();  
    while (millis()<timeDelay +7000)
    {digitalWrite(valvePin, HIGH);}
    sleepEnable = true;
    Serial.println("closing valve");
    digitalWrite(valvePin, LOW);
    Serial.println("NPK NOT flowing");
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("pump/full"); //change this
      client.subscribe("pump/half"); //change this
      client.subscribe("valve/10");  //change this
      client.subscribe("valve/5");   //change this
      Serial.println("subscribed to pump and valve");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// end of MQTT PART //

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
  if (!MDNS.begin(host))
  {
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println("mDNS responder started");
  Serial.printf("test serial");
  WiFi.mode(WIFI_STA);

  AsyncWiFiManager wm(&server, &dns);
  // wm.resetSettings();
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
  pinMode(valvePin, OUTPUT);
  pinMode(pumpPin, OUTPUT);

   digitalWrite(pumpPin, LOW);
   digitalWrite(valvePin, LOW);

  // For MQTT
  /* set SSL/TLS certificate */
  espClient.setCACert(ca_cert);
  espClient.setCertificate(device001_cert);
  espClient.setPrivateKey(device001_key);

  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);

  // Deep sleep enabling
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  sleepTime = millis();
  //deep sleep end
}

long lastupdate;
int threshold_N = 25, threshold_P = 9, threshold_K = 5, threshold_soil_moisture = 20;
bool initflag = true;
bool readNPK = false;

float rawVoltsValue;
float solarVoltage;

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");

  if (millis() > lastupdate + 5000)
  {
    digitalWrite(pumpPin, LOW);
    digitalWrite(valvePin, LOW);
    lastupdate = millis();

    reading_soil_moisture = soilMoisture(analogSoilMoistureInPin);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    reading_air_humidity = dht.readHumidity();
    //reading_air_humidity = reading_soil_moisture + 45;
    // Read temperature as Celsius
    reading_air_temp = dht.readTemperature();
    // reading_air_temp =reading_soil_moisture + 60;
    Serial.print("Air temperature and Humidity: ");
    Serial.print(reading_air_humidity);
    Serial.print(" ");
    Serial.println(reading_air_temp);

    // generating values for N, P, K parameters
    rawVoltsValue = analogRead(solarVoltPin);
    solarVoltage = (rawVoltsValue * 6) / 4096;

    Serial.print("analog value of pin value :");
    Serial.println(rawVoltsValue);
    Serial.print("in terms of Volts pin value :");
    Serial.println(solarVoltage);
    if (solarVoltage >= 2.25)
    {
      delay(250);
      reading_N = nitrogen(&mod);
      delay(250);
      reading_P = phosphorous(&mod);
      delay(250);
      reading_K = potassium(&mod);
      delay(250);

      readNPK = true;
      printNPK(reading_N, reading_P, reading_K);
    }
    // publishing the values to MQTT

    static char reading_air_temp_str[10] = "\0";
    static char reading_air_humidity_str[10] = "\0";
    static char reading_soil_moisture_str[10] = "\0";
    itoa(reading_air_temp, reading_air_temp_str, 10);
    itoa(reading_air_humidity, reading_air_humidity_str, 10);
    itoa(reading_soil_moisture, reading_soil_moisture_str, 10);
    // air param
    if (reading_air_humidity < 100 && reading_air_temp < 100)
    {
      client.publish("littlefarm/temp", reading_air_temp_str);
      client.publish("littlefarm/hum", reading_air_humidity_str);
    }
    else
    {
      client.publish("littlefarm/msg", "out of threshold");
    }

    // // soil param
    client.publish("littlefarm/mois", reading_soil_moisture_str);

    if (readNPK)
    {
      static char reading_N_str[10] = "\0";
      static char reading_P_str[10] = "\0";
      static char reading_K_str[10] = "\0";
      itoa(reading_N, reading_N_str, 10);
      itoa(reading_P, reading_P_str, 10);
      itoa(reading_K, reading_K_str, 10);

      client.publish("littlefarm/n", reading_N_str);
      client.publish("littlefarm/p", reading_P_str);
      client.publish("littlefarm/k", reading_K_str);

      if ((threshold_N - 8 > reading_N) && (threshold_P - 3 > reading_P) && (threshold_K - 3 > reading_K))
      {
        client.publish("littlefarm/msg", "Deficit : Nitrogen, Potassium and Phosphorous.  Suggested to add NPK");
      }
      else if (threshold_N - 8 > reading_N)
      {
        client.publish("littlefarm/msg", "Deficit : Nitrogen. Suggested to add NPK");
      }
      else if (threshold_P - 3 > reading_P)
      {
        client.publish("littlefarm/msg", "Deficit : Phosphorous.  Suggested to add NPK");
      }
      else if (threshold_K - 3 > reading_K)
      {
        client.publish("littlefarm/msg", "Deficit : Potassium.  Suggested to add NPK");
      }
      readNPK = false;
    }
    // Deep sleep after 60s of bing on
    //Serial.println("1 min gap for actuation");
    client.publish("littlefarm/awake", "awake");
    if (sleepEnable && (millis() > sleepTime + 60000) )
    {
      client.publish("littlefarm/sleep", "sleep");
      Serial.println("going to sleep");
      delay(2000);
      esp_deep_sleep_start();
    }
  }
}