#include "Arduino.h"
#ifdef ARDUINO_ARCH_ESP32
//#include <analogWrite.h>
#define FREQ          30000
#define RESOLUTION    8
static int pwmChannel = 0;
#endif
class Motor {
  private:
    int speedPin;
    int dirPin;
    int speed;
    const int fwd = LOW;
    const int bkd = !fwd;
#ifdef ARDUINO_ARCH_ESP32
    int motorChannel = pwmChannel++;
    void analogWrite(int pin, int speed) const {
      ledcWrite(motorChannel, speed);
    }
#endif

  public:
    Motor(int speedPin, int dirPin, int speed = 255)
    : speedPin(speedPin)
    , dirPin(dirPin)
    , speed(speed) {
      pinMode(speedPin, OUTPUT);
      pinMode(dirPin, OUTPUT);
#ifdef ARDUINO_ARCH_ESP32
      // configure LED PWM functionalitites
      ledcSetup(motorChannel, FREQ, RESOLUTION);

      // attach the channel to the GPIO to be controlled
      ledcAttachPin(speedPin, motorChannel);
#endif
      analogWrite(speedPin, 0);
    }
    void moveForward() const;
    void moveBackward() const;
    void stop() const;
};

class MotorSet {
  private:
    const Motor& leftMotor;
    const Motor& rightMotor;
  
  public:
    MotorSet(const Motor& left, const Motor& right) : leftMotor(left), rightMotor(right){}
    void moveForward();
    void moveBackward();
    void stop();
    void rotateDirection(int direction);
};
