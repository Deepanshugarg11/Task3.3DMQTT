#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "IPHONE12";
const char* password = "12345678";

// MQTT Broker details
const char* mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;
const char* topic_wave = "SIT210/wave";
const char* topic_pat = "SIT210/pat";

// Ultrasonic sensor pins
const int trigPin = 9;
const int echoPin = 10;
// LED pin
const int ledPin = 13;

long duration;
int distance;

// WiFi and MQTT client setup
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);
  
  // Set pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  // Connect to WiFi
  connectWiFi();
  
  // Set MQTT broker and callback
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqttCallback);
  
  // Connect to MQTT broker
  connectMQTT();
}

void connectWiFi() {
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ArduinoClient")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topic_wave);
      client.subscribe(topic_pat);
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(500);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message received from topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);
  
  if (String(topic) == "SIT210/wave") {
    flashLED(3); // Flash LED 3 times for "wave"
  } else if (String(topic) == "SIT210/pat") {
    flashLED(5); // Flash LED 5 times for "pat"
  }
}

void detectWave() {
  // Send pulse from ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo pulse duration
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in centimeters
  distance = duration * 0.034 / 2;
  
  // If distance is less than 20 cm, consider it a "wave"
  if (distance < 20) {
    Serial.println("Wave detected! Publishing message...");
    client.publish(topic_wave, "your-name");  // Publish your name to the topic
  }
}

void flashLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}

void loop() {
  // Detect wave
  detectWave();
  
  // Ensure MQTT communication is handled
  client.loop();
  
  // Reconnect if disconnected
  if (!client.connected()) {
    connectMQTT();
  }
}
