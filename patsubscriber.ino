#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "inder's Galaxy";
const char* password = "fesecurity";

// MQTT Broker details
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* wave_topic = "SIT210/wave";  // Topic for wave messages
const char* pat_topic = "SIT210/pat";    // Topic for pat messages
const char* confirm_topic = "SIT210/confirm"; // Topic for confirmation messages

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
  String receivedMessage = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    receivedMessage += (char)message[i];
  }
  Serial.println();
  
  // Handle wave and pat messages
  if (String(topic) == wave_topic) {
    Serial.println("Wave message received");
    // Flash LED 2 times for wave
    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
    }
    // Publish confirmation for wave
    client.publish(confirm_topic, "wave from Deepanshu received");
    Serial.println("wave from Deepanshu received - confirmation sent");
  } else if (String(topic) == pat_topic) {
    Serial.println("Pat message received");
    // Flash LED 4 times for pat
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
    // Publish confirmation for pat
    client.publish(confirm_topic, "pat from Deepanshu received");
    Serial.println("pat from Ansh received - confirmation sent");
  }
}

void reconnect() {
  // Reconnect to the MQTT broker if disconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoSubscriber")) {
      Serial.println("connected");
      // Subscribe to wave and pat topics
      client.subscribe(wave_topic);
      client.subscribe(pat_topic);
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