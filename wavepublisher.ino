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

// Ultrasonic sensor pins
const int trigPin = 9;
const int echoPin = 8;

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

void reconnect() {
  // Reconnect to the MQTT broker if disconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoPublisher")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  // Setup ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  // Send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure distance
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  
  // If object is within 50 cm, publish message
  if (distance <= 15.0) {
    Serial.println("Wave detected, publishing message...");
    client.publish(topic, "Deepanshu");
    delay(2000); // Delay to avoid spamming the message
  }
}