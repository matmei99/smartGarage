#include <SoftwareSerial.h> // To use with arduino uno or nano etc...
#include <string.h>

#include <ArduinoMqttClient.h>
//For Arduino Uno Wifi Rev2:
#include <WiFiNINA.h>

#define MAXDATA 78
#define TIMEOUT 10

int op =0;

SoftwareSerial BlueTooth(2, 3);

char ssid[] = "SmartGarage";
char pass[] = "smartgarage";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.0.169";
int port = 1883;
const char topic[] = "/garage_command";

//before sending command AT+DISI?
// AT+IMME1 and AT+ROLE1 MUST be sent
//to setup the HM10
#define COMMAND "AT+DISI?"

int prepare = 0;
int myRSSI;
long timeoutOver;
char data[MAXDATA];
int timeOut = 0;
char FINDER = '4';

void setup()
{
  Serial.begin(9600);
  

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(2000);
  }
  Serial.println("Connected to WiFi...");

  while (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(1000);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  BlueTooth.begin(9600);

  delay(1000);
  Serial.print("Initialize");

  BlueTooth.write("AT+DISI?");
  delay(600);

}

void loop()
{
  boolean somethingReceived = false;
  bool userIsNear = false;
  int pos = 0;

  char data[MAXDATA];

  if (BlueTooth.available())
  {

    somethingReceived = true;
    do
    {
      while (BlueTooth.available())
      {
        data[min(pos, MAXDATA - 1)] = BlueTooth.read();
        timeoutOver = millis() + TIMEOUT;
        pos++;
      }
      //Serial.println(pos);
      //Serial.println(data);
    }
    while (millis() < timeoutOver);
    data[pos] = '\0';
  }

  if (somethingReceived)
  {
    if (strstr(data, "DISC:00000000:00000000000000000000000000000000")) {
    }
    else {
      if (strstr(data, "6A9B6985D72747C28B893983D32275BD")&&op!=1)
      {
        Serial.println("Connected");
        mqttClient.poll();

        String command = "open";

        if (command != "") {
          mqttClient.beginMessage(topic);
          mqttClient.print(command);
          mqttClient.endMessage();

          Serial.println("Published '" + command + "' on topic" + topic);
          op = 1;
        }
      }
    }
  }
  delay(10);
  BlueTooth.write("AT+DISI?");
  delay(600);

}
