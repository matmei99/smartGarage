#define BLYNK_TEMPLATE_ID "TMPL-wCZlFE1"
#define BLYNK_DEVICE_NAME "Smart Garage"
#define BLYNK_AUTH_TOKEN "0Lj-wSZEv3mrvyjTDBFZCRahdGx1Ddq5"

#define BLYNK_PRINT Serial

#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoMqttClient.h>
#include <BlynkSimpleWiFiShield101.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SmartGarage";                 // your network SSID (name)
char pass[] = "smartgarage";                 // your network key

int val0 = 0, val1 = 0;

int wifi_status = WL_IDLE_STATUS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.169";
int port = 1883;
const char topic[] = "/garage_command";

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
    //check for lightsensor
    //closeGarage();
  }

}

//OPEN GARAGE FROM BLYNK APP
BLYNK_WRITE(V1)
{
  val1 = param.asInt();
  if (val1 == 1) {
    Serial.println("Open Garage...");
    //openGarage();
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  Serial.print("Connecting...");
  Blynk.begin(auth, ssid, pass);
  Serial.println("done");

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

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topic);

}

void loop()
{   
  Blynk.run();
  mqttClient.poll();
}

void onMqttMessage(int messageSize) {
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  String command;

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    command += (char)mqttClient.read();
  }

  Serial.println("Command: " + command);
  
}
