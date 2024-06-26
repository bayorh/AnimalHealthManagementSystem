#include <WiFi.h>
#include <aWOT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

WiFiServer server(80);
Application app;
DynamicJsonDocument json(1024);

//
// wifi credential
const char *ssid = "";
const char *password = "";

// Wifi implementation
void ConnectWifi()
{
  const int Wifiled = 2;
  pinMode(Wifiled, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".....");
    digitalWrite(Wifiled, WL_CONNECTED);
    delay(300);
    digitalWrite(Wifiled, !WL_CONNECTED);
    delay(300);
  }
  digitalWrite(Wifiled, WL_CONNECTED);
  Serial.println("Connected to ");
  Serial.println(WiFi.localIP());
}

// Temperature credential
const int oneWireBus = 15;
OneWire oneWire(oneWireBus);

// Temperature value
String GetTemperatureValues()
{
  DallasTemperature sensors(&oneWire);

  sensors.begin();
  sensors.requestTemperatures();

  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);

  // turning data recieved int json format
  json["TempInCValue"] = temperatureC;
  json["UnitC"] = "ºC";
  json["TempInFValue"] = temperatureF;
  json["UnitF"] = "ºF";

  char json_result[256];
  serializeJson(json, json_result);
  Serial.println(json_result);
  return json_result;
}
void index(Request &req, Response &res)
{
  res.set("Access-Control-Allow-Origin", "*");
  res.set("Access-Control-Allow-Methods", "GET");
  res.print("Welcome to Animal Care");
}
void TempValue(Request &req, Response &res)
{
  String tempResult = GetTemperatureValues();

  // Convert String to const char*
  const char *jsonCString = tempResult.c_str();

  // Set the content type to JSON
  res.set("Content-Type", "application/json");

  // Send the JSON string as response
  res.print(jsonCString);
}

void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  app.get("/", &index);
  app.get("/temp", &TempValue);
  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client.connected())
  {
    app.process(&client);
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    ConnectWifi();
  }
}