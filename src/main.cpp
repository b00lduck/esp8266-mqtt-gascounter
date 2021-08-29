#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

IPAddress mqttServer(192, 168, 2, 100);
const char *clientId = "ESP8266Client-gascounter";

const int gasPin = 2;

void callback(char* topic, byte* payload, unsigned int length) {}

WiFiClient wifiClient = WiFiClient();
PubSubClient mqttClient(mqttServer, 1883, callback, wifiClient);

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("Connected - %s\n", WiFi.localIP().toString().c_str());
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker..");    
    if (mqttClient.connect(clientId)) {
      Serial.println("Connected.");
    }
  }
}

void setup() {
  pinMode(gasPin, INPUT_PULLUP);
  Serial.begin(9600);
  connectToWiFi();
}

int gasState = 0;
int lastGasState = 0;

void loop() {

  if (!mqttClient.connected()) {
    reconnect();
  }
    
  mqttClient.loop();

  gasState = digitalRead(gasPin);

  // compare the gasState to its previous state
  if (gasState != lastGasState) {
    // if the state has changed, increment the counter
    if (gasState == LOW) {
      Serial.println("Publishing data..");
      mqttClient.publish("energy/gas", "increment=0.1");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastGasState = gasState;
}