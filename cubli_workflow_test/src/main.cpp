#include "Arduino.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "Tplink216";
const char* password = "tryngu3ssthi5";

#define MOTOR1_PWM 18
#define MOTOR1_DIR 5

#define MOTOR2_PWM 17
#define MOTOR2_DIR 16

#define MOTOR3_PWM 32
#define MOTOR3_DIR 4

#define BRAKE_RELEASE 26

#define TEST_DURATION_MS 1000
#define TEST_SPEED 100   // 0-255, adjust as needed

struct Motor {
  uint8_t pwmPin;
  uint8_t dirPin;
  uint8_t pwmChannel;
  const char* name;
};

Motor motors[] = {
  { MOTOR1_PWM, MOTOR1_DIR, 0, "Motor 1" },
  { MOTOR2_PWM, MOTOR2_DIR, 1, "Motor 2" },
  { MOTOR3_PWM, MOTOR3_DIR, 2, "Motor 3" },
};

const int numMotors = sizeof(motors) / sizeof(motors[0]);

void runMotor(Motor &m, bool forward, int durationMs) {
  Serial.printf("%s - %s\n", m.name, forward ? "FORWARD" : "REVERSE");
  digitalWrite(m.dirPin, forward ? HIGH : LOW);
  ledcWrite(m.pwmChannel, 255 - TEST_SPEED);
  delay(durationMs);
  ledcWrite(m.pwmChannel, 255);
  delay(200);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected, IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("esp32-motors");
  // ArduinoOTA.setPassword("somepassword");

  ArduinoOTA
    .onStart([]() { Serial.println("OTA start"); })
    .onEnd([]() { Serial.println("\nOTA end"); })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]\n", error);
    });

  ArduinoOTA.begin();

  // Release brakes
  pinMode(BRAKE_RELEASE, OUTPUT);
  digitalWrite(BRAKE_RELEASE, HIGH);

  // Set up motor pins
  for (int i = 0; i < numMotors; i++) {
    pinMode(motors[i].dirPin, OUTPUT);
    ledcSetup(motors[i].pwmChannel, 20000, 8);   // 20kHz PWM, 8-bit resolution
    ledcAttachPin(motors[i].pwmPin, motors[i].pwmChannel);
  }
}

void loop() {
  ArduinoOTA.handle();
  
  for (int i = 0; i < numMotors; i++) {
    runMotor(motors[i], true, TEST_DURATION_MS);
    // ArduinoOTA.handle();
    // runMotor(motors[i], false, TEST_DURATION_MS);
    ArduinoOTA.handle();
  }
}