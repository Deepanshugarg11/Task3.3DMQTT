#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "inder's Galaxy";
const char* password = "fesecurity";

// MQTT Broker details
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* topic = "SIT210/wave"; // MQTT topic

WiFiClient espClient;
PubSubClient client(espClient);

// LED pin
const int ledPin = 7;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // Connect to Wi-Fi
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
  
  // Flash the LED 3 times when a message is received
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

void reconnect() {
  // Reconnect to the MQTT broker if disconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoSubscriber")) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  // Setup LED pin
  pinMode(ledPin, OUTPUT);
  
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
}