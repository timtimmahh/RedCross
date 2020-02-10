#include "Arduino.h"
#include "Motor.h"

void MotorSet::moveForward(const SpeedValue& leftSpeed, const SpeedValue& rightSpeed) {
  _leftMotor.moveForward(leftSpeed);
  _rightMotor.moveForward(rightSpeed);
}

void MotorSet::moveBackward(const SpeedValue& leftSpeed, const SpeedValue& rightSpeed) {
  _leftMotor.moveBackward(leftSpeed);
  _rightMotor.moveBackward(rightSpeed);
}

void MotorSet::stop(bool brake) {
  _leftMotor.stop(brake);
  _rightMotor.stop(brake);
}

void MotorSet::rotateDirection(uint8_t direction) {
	if (direction < 0) {
		_leftMotor.moveBackward();
		_rightMotor.moveForward();
	} else {
		_rightMotor.moveBackward();
		_leftMotor.moveForward();
	}
}


