#include "MotorMovement.h"

void Motor::moveForward() const {
  digitalWrite(dirPin, fwd);
  analogWrite(speedPin, speed);
}

void Motor::moveBackward() const {
  digitalWrite(dirPin, bkd);
  analogWrite(speedPin, speed);
}

void Motor::stop() const {
  analogWrite(speedPin, 0);
}


