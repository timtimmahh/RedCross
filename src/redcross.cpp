#include "redcross.h"
#include "RealTime.h"
#include "SensorServer.h"
#include "MotorMovement.h"
#include "LidarMap.h"
#include "AccelGyro.h"
// ESP32 configuration
#ifdef ARDUINO_ARCH_ESP32
// Use pointers to manually manage storage
SensorServer<LidarMap, AccelGyro> *server;
//MotorSet motors(Motor(18, 19), Motor(17, 16));
LidarMap *lidar;
AccelGyro *ag;

// Arduino UNO configuration
#elif ARDUINO_AVR_UNO
#include <SoftwareSerial.h>
MotorSet motors(Motor(6, 7), Motor(5, 4));
SoftwareSerial sw(0, 1); // RX, TX

LIDARLite lidarLite;

#endif

void setup() {
  Serial.begin(BAUD);
  delay(2000);
  Serial.println("Starting...");
  // ESP32 setup
#ifdef ARDUINO_ARCH_ESP32
  Serial.println("ESP32 Setup");
  // start WiFi for ESP32
  setupWiFi();
  // create sensor instances on the heap
  lidar = new LidarMap();
  ag = new AccelGyro();
  // tell the WebServer which sensors to display
  server = new SensorServer<LidarMap, AccelGyro>(SERVER_PORT, *lidar,*ag);
  // set up the Garmin Lidar-Lite sensor
//  motors.moveForward();
#elif ARDUINO_AVR_UNO
  Serial.println("UNO Setup");
  // set up the Garmin Lidar-Lite sensor
  lidarLite.begin(0, true);
  lidarLite.configure(0);
//  sw.begin(115200);
  ag.start();

#endif
}

int direction = 0;
unsigned long debugTime = 2000, lastPrint = 0;

void loop() {
#ifdef ARDUINO_ARCH_ESP32
  // handles client connections
  server->handleClient();
//  const Coords& accRot = ag->getData();
//  Serial.print("AccelRot: ");
//  Serial.println(accRot.toString());
//  double distance = lidar->getData();
//  Serial.print("Distance: ");
//  Serial.println(distance);
//  if (distance < 20) {
//    motors.moveBackward();
//    direction = 0;
//  } else if (!direction) {
//    motors.rotateDirection(1);
//    delay(2000);
//  }
//  if (!direction) {
//    motors.moveForward();
//    direction = 1;
//  }
  delay(1);
#elif ARDUINO_AVR_UNO
  double distance = getDistance(100);
//  if (distance < 20) {
//    motors.moveBackward();
//    direction = 0;
//  } else if (!direction) {
//    motors.rotateDirection(1);
//    delay(2000);
//  }
//  if (!direction) {
//    motors.moveForward();
//    direction = 1;
//  }
  if (Serial.available()) {
    const char *msg = (String(distance) + ";").c_str();
    LOG_D("UNO - Sending Msg: '" + msg + "'");
    Serial.write(msg);
  } else
    LOG_D("UNO - ESP unavailable.");

  if (millis() - lastPrint > debugTime) {
    Serial.println(String("Distance: ") + distance);
    Serial.print(String("Acceleration: ") + ag.getAcceleration().toString());
    Serial.println(String(" Rotation: ") + ag.getRotation().toString());
    lastPrint = millis();
  }
#endif
}
