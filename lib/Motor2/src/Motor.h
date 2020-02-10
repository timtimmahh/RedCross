#include "Arduino.h"
#include "SpeedValue.h"
#ifdef ARDUINO_ARCH_ESP32
#include "analogWrite.h"
#endif

#define DEFAULT_SPEED   SpeedNative(255)

class WheelSpecs {
public:
  double radius;
  double width;
  double circumference;
  WheelSpecs(double radius, double width) : radius(radius), width(width), circumference(2 * radius * PI) {}
};

class Motor {
private:
  int _speedPin;
  int _dirPin;
  int _brakePin;
  int _currentPin;

  double speedNativeUnits(const SpeedValue& speed) const {
    double divisor;
    switch (speed.getSpeedUnit()) {
      case PERCENT:divisor = 100.0;
        break;
      case RPS:divisor = this->getMaxRPS();
        break;
      case RPM:divisor = this->getMaxRPM();
        break;
      case DPS:divisor = this->getMaxDPS();
        break;
      case DPM:divisor = this->getMaxDPM();
        break;
      case NATIVE:
      default:return speed.toNativeUnits();
    }
    return speed.toNativeUnits() / divisor * this->getMaxSpeed();
  }

  void runToAbsPos(double position);
  void runToRelPos(double position);
  void runTimed(long time);
protected:
  const SpeedValue& _speed;
  int _count_per_rot{};
  int _count_per_m{};
  uint8_t polarity{};
  double _position{};
  double _state{};
  double _maxSpeed = 255;
  double _maxDPS{};
  double _maxDPM{};
  double _maxRPS{};
  double _maxRPM{};
  WheelSpecs *_wheelSpecs;

public:
  Motor(int speedPin, int dirPin, int brakePin = -1, int currentPin = -1,
        const SpeedValue& speed = DEFAULT_SPEED, WheelSpecs *wheelSpecs = nullptr) :
      _speedPin(speedPin),
      _dirPin(dirPin),
      _brakePin(brakePin),
      _currentPin(currentPin),
      _speed(speed),
      _wheelSpecs(wheelSpecs) {
    pinMode(speedPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    analogWrite(speedPin, 0);
    if (brakePin != -1) {
      pinMode(brakePin, OUTPUT);
      analogWrite(brakePin, 0);
    }
    if (currentPin != -1) {
      pinMode(currentPin, INPUT);
    }
  }

  SpeedValue getSpeed() const { return _speed; }
  double getPosition() const { return _position; }
  double getMaxSpeed() const { return _maxSpeed; }
  double getMaxDPS() const { return _maxDPS; }
  double getMaxDPM() const { return _maxDPM; }
  double getMaxRPS() const { return _maxRPS; }
  double getMaxRPM() const { return _maxRPM; }
  double getRotations() const { return _position / _count_per_rot; }
  double getDegrees() const { return getRotations() * 360; }

  void moveForward(const SpeedValue& speed = DEFAULT_SPEED) const;
  void moveBackward(const SpeedValue &speed = DEFAULT_SPEED) const;
  void move(const SpeedValue& speed = DEFAULT_SPEED, uint8_t direction = 1) const;
  void stop(bool brake = false) const;
  void on(SpeedValue speed, bool brake = true, bool block = false);
  void off(bool brake = true);
  void onForRotations(SpeedValue speed, int rotations, bool brake = true, bool block = true);
  void onForDegrees(SpeedValue speed, int degrees, bool brake = true, bool block = true);
  void onToPosition(SpeedValue speed, double position, bool brake = true, bool block = true);
  void onForSeconds(SpeedValue speed, long seconds, bool brake = true, bool block = true);
};

class MotorSet {
private:
  const Motor& _leftMotor;
  const Motor& _rightMotor;
  double _wheelDistance;
public:
  MotorSet(const Motor& left, const Motor& right, double wheelDistance = -1)
      : _leftMotor(left), _rightMotor(right), _wheelDistance(wheelDistance) {}
  MotorSet(int speedPin1, int dirPin1, int speedPin2, int dirPin2, const SpeedValue &speed, WheelSpecs *wheelSpecs)
      : MotorSet(Motor(speedPin1, dirPin1, -1, -1, speed, wheelSpecs),
                 Motor(speedPin2, dirPin2, -1, -1, speed, wheelSpecs)) {}
  void moveForward(const SpeedValue& leftSpeed = DEFAULT_SPEED, const SpeedValue& rightSpeed = DEFAULT_SPEED);
  void moveBackward(const SpeedValue& leftSpeed = DEFAULT_SPEED, const SpeedValue& rightSpeed = DEFAULT_SPEED);
  void stop(bool brake = false);
  void rotateDirection(uint8_t direction);
};