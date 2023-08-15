#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// MQTT broker
const char* mqtt_server = "your_MQTT_broker_server";
const int mqtt_port = 1883;
const char* mqtt_user = "your_MQTT_username";
const char* mqtt_password = "your_MQTT_password";

// LED pin
const int LED_PIN = 13;

// Motion sensor pin
const int MOTION_PIN = 12;

// Light intensity sensor pin
const int LIGHT_PIN = A0;

// Thresholds
const int MOTION_THRESHOLD = 500; // adjust this based on your sensor
const int LIGHT_THRESHOLD = 500; // adjust this based on your sensor

// Initialize WiFi client and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);

  // Set motion sensor pin as input
  pinMode(MOTION_PIN, INPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("smart-street-light", mqtt_user, mqtt_password)) {
      client.subscribe("street-light");
    }
    else {
      delay(1000);
    }
  }
}

void loop() {
  // Check for MQTT messages
  client.loop();

  // Check for motion detection
  if (analogRead(MOTION_PIN) > MOTION_THRESHOLD) {
    // Check light intensity
    int light_intensity = analogRead(LIGHT_PIN);
    if (light_intensity < LIGHT_THRESHOLD) {
      // Turn on LED
      digitalWrite(LED_PIN, HIGH);
      // Publish MQTT message
      client.publish("street-light", "on");
    }
  }
  else {
    // Turn off LED
    digitalWrite(LED_PIN, LOW);
    // Publish MQTT message
    client.publish("street-light", "off");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Parse message and control LED
  if (message == "on") {
    // Turn on LED
    digitalWrite(LED_PIN, HIGH);
    // Publish MQTT message
    client.publish("street-light", "on");
  }
  else if (message == "off") {
    // Turn off LED
    digitalWrite(LED_PIN, LOW);
    // Publish MQTT message
    client.publish("street-light", "off");
  }
}
