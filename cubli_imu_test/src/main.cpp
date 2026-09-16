#include "Arduino.h"
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoOTA.h>

const char* ssid = "Tplink216";
const char* password = "tryngu3ssthi5";

const char* mqtt_server = "192.168.0.150";  // your laptop's IP running Mosquitto
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/accel";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
Adafruit_MPU6050 mpu;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("esp32-imu")) {
      Serial.println("connected");
    } else {
      Serial.printf("failed, rc=%d, retrying in 2s\n", mqttClient.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);

  ArduinoOTA.setHostname("esp32-imu");
  ArduinoOTA.begin();

  Wire.begin();  // default SDA=21, SCL=22

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(1000); }
  }
  Serial.println("MPU6050 found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  ArduinoOTA.handle();   // add this

  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  char payload[160];
  snprintf(payload, sizeof(payload),
    "{\"t\":%lu,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f}",
    millis(),
    a.acceleration.x, a.acceleration.y, a.acceleration.z,
    g.gyro.x, g.gyro.y, g.gyro.z);


  mqttClient.publish(mqtt_topic, payload);
  Serial.println(payload);

  delay(10);  // ~100 Hz publish rate, adjust as needed
}