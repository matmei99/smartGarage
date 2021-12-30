#include <ArduinoMqttClient.h>
//For Arduino Uno Wifi Rev2:
#include <WiFiNINA.h>
//For Arduino MKR1000:
//#include <WiFi101.h>

char ssid[] = "SmartGarage";
char pass[] = "smartgarage";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.169";
int port = 1883;
const char topic[] = "/garage_command";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){
    
  }

  Serial.println("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pass);
    delay(2000);
  }
  Serial.println("Connected to WiFi...");

  if(!mqttClient.connect(broker, port)){
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:

  mqttClient.poll();

  String command = Serial.readString();

  if(command != ""){
    mqttClient.beginMessage(topic);
    mqttClient.print(command);
    mqttClient.endMessage();

    Serial.println("Published '" + command + "' on topic" + topic);
  }
}
