#include "Motor.h"

void Motor::moveForward(const SpeedValue& speed) const {
	move(speed, LOW);
}

void Motor::moveBackward(const SpeedValue& speed) const {
	move(speed, HIGH);
}

void Motor::move(const SpeedValue& speed, uint8_t direction) const {
	digitalWrite(_dirPin, direction);
	analogWrite(_speedPin, speedNativeUnits(speed));
}

void Motor::stop(bool brake) const {
	analogWrite(_speedPin, 0);
	if (brake && _brakePin != -1)
		analogWrite(_brakePin, HIGH);
}

void Motor::runToAbsPos(double position) {

}

void Motor::runToRelPos(double position) {

}
void Motor::runTimed(long time) {

}
void Motor::on(SpeedValue speed, bool brake, bool block) {
  if (speed > 0)
    moveForward(speed);
  else moveBackward(speed);
}
void Motor::off(bool brake) {
  stop(brake);
}
void Motor::onForRotations(SpeedValue speed, int rotations, bool brake, bool block) {

}
void Motor::onForDegrees(SpeedValue speed, int degrees, bool brake, bool block) {

}
void Motor::onToPosition(SpeedValue speed, double position, bool brake, bool block) {

}
void Motor::onForSeconds(SpeedValue speed, long seconds, bool brake, bool block) {

}
