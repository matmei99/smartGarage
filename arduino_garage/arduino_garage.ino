#define BLYNK_TEMPLATE_ID "TMPL-wCZlFE1"
#define BLYNK_DEVICE_NAME "Smart Garage"
#define BLYNK_AUTH_TOKEN "0Lj-wSZEv3mrvyjTDBFZCRahdGx1Ddq5"
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoMqttClient.h>
#include <BlynkSimpleWiFiShield101.h>
#include "Ultrasonic.h"
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

//ULTRASONIC VALUES
int trigPin = 5;
int echoPins[] = {2, 3, 4};
int numSensors = 3;
bool carInGarage = false;
Ultrasonic ultrasonic(trigPin, echoPins, numSensors);

//BLYNK
char auth[] = BLYNK_AUTH_TOKEN;
int val0 = 0, val1 = 0;

//WIFI AND MQTT
char ssid[] = "SmartGarage";
char pass[] = "smartgarage";
int wifi_status = WL_IDLE_STATUS;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
const char broker[] = "192.168.0.169";
int port = 1883;
const char topic[] = "/garage_command";

//AC MOTOR
int r1 = 0;
int r2 = 1;

//LIGHT SENSOR
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
bool carInLightsensor = false;
sensors_event_t event;

// Called when Blynk is connected
BLYNK_CONNECTED() {
  // request the values on connect
  Blynk.syncAll();

  Blynk.setProperty(V1, "onLabel", "OPEN");
  Blynk.setProperty(V1, "offLabel", "OPEN");

  Blynk.setProperty(V0, "onLabel", "CLOSE");
  Blynk.setProperty(V0, "offLabel", "CLOSE");
}

//CLOSE GARAGE FROM BLYNK APP
BLYNK_WRITE(V0)
{
  val0 = param.asInt();
  if (val0 == 1) {
    Serial.println("Close Garage...");
    if (!carInLightsensor) {
      closeGarage();
    }
  }

}

//OPEN GARAGE FROM BLYNK APP
BLYNK_WRITE(V1)
{
  val1 = param.asInt();
  if (val1 == 1) {
    Serial.println("Open Garage...");
    openGarage();
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  //while (!Serial);

  //Connecting to Blynk
  Serial.print("Connecting...");
  Blynk.begin(auth, ssid, pass);
  Serial.println("done");

  //Connecting to WIFI
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.print(".");
    delay(5000);
  }
  Serial.println("You're connected to the network");
  Serial.println();

  //Connect to MQTT Broker on Raspberry
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  while (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    delay(1000);
  }
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topic);

  //AC Motor
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);

  //Light Sensor
  while (!tsl.begin())
  {
    Serial.println("no TSL2561 detected");
    delay(1000);
  }
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

  Serial.println("Setup finished");

}

void loop()
{
  Blynk.run();
  mqttClient.poll();
  measureUSS();
  lightSensor();
  delay(100);         //Delay required for Ultrasonic Sensors
}

//When a MqttMessage is received...
void onMqttMessage(int messageSize) {
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  String command;

  //if /garage_command is used, check for open or close command
  if (mqttClient.messageTopic() == "/garage_command") {
    while (mqttClient.available()) {
      command += (char)mqttClient.read();
    }
    if (command == "open") {
      if (!carInGarage) {
        openGarage();
      }
      Serial.println("Command: " + command);
    }
    if (command == "close") {
      if (!carInLightsensor) {
        closeGarage();
      }
      Serial.println("Command: " + command);
    }
  }

}

//Check if a car is already inside the garage
void measureUSS() {
  int distances[numSensors];
  ultrasonic.measureDistances(distances);

  if (distances[0] < 20 || distances[1] < 20 || distances[2] < 20) {
    if (!carInGarage) {
      Serial.println("Car in Garage");
    }
    carInGarage = true;
  } else {
    if (carInGarage) {
      Serial.println("Car not in Garage");
    }
    carInGarage = false;
  }
}

//Open and close the garage
void openGarage() {
  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
}
void closeGarage() {
  digitalWrite(r2, HIGH);
  digitalWrite(r1, LOW);
}

//Check if a car is standing in the door line
void lightSensor() {
  tsl.getEvent(&event);
  if (event.light)
  {
    carInLightsensor = checkCarInLine(event);
    Serial.print(event.light);
    Serial.println(" lux");
    if (carInLightsensor == true)
    {
      Serial.println("Stopped closing Door");
    }
  } else
  {
    Serial.println("Sensor overload");
  }
}

bool checkCarInLine(sensors_event_t event)
{
  if (event.light <= 200)
  {
    if (!carInLightsensor) {
      Serial.println("Car in Line");
      openGarage();
    }
    return true;

  }
  else
  {
    if (carInLightsensor) {
      Serial.println("Car not in Line");
    }
    return false;
  }
}
