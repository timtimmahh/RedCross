//
// Created by tim on 2/2/20.
//

#ifndef REDCROSS_SPEEDVALUE_H
#define REDCROSS_SPEEDVALUE_H

#include "Arduino.h"

/**
 * Enum specifying the units to use for speed calculation.
 */
enum SpeedUnit { NATIVE, PERCENT, RPS, RPM, DPS, DPM };

/**
 * Unit names for outputting to the console.
 */
static const char *unitNames[] = {"counts/sec", "%", "rot/sec", "rot/min", "deg/sec", "deg/min"};

/**
 * A class specifying a speed value using a SpeedUnit specifier.
 */
class SpeedValue : public Printable {
private:
protected:
  /**
   * The speed value.
   */
  double value;
  /**
   * The SpeedUnit specifying the speed value.
   */
  const SpeedUnit _speedUnit;
public:
  /**
   * Constructor for specifying the value and type of units.
   *
   * @param value the speed value
   * @param unit the speed units
   */
  explicit SpeedValue(double value, SpeedUnit unit)
      : value(value), _speedUnit(unit) {}

  SpeedUnit getSpeedUnit() const { return _speedUnit; }

  /**
   * Converts the speed value to native units.
   *
   * @param motor the motor to help with conversion (if necessary)
   * @return the native speed value
   */
  double toNativeUnits() const {
    return value;
  }

  bool operator==(SpeedValue &other);

  bool operator==(double other);

  bool operator!=(SpeedValue &other);

  bool operator!=(double other);

  bool operator<(SpeedValue &other);

  bool operator<(double other);

  bool operator<=(SpeedValue &other);

  bool operator<=(double other);

  bool operator>(SpeedValue &other);

  bool operator>(double other);

  bool operator>=(SpeedValue &other);

  bool operator>=(double other);

  SpeedValue operator*(const SpeedValue &other);

  SpeedValue operator*(double other);

  SpeedValue operator/(const SpeedValue &other);

  SpeedValue operator/(double other);

  SpeedValue operator+(const SpeedValue &other);

  SpeedValue operator+(double other);

  SpeedValue operator-(const SpeedValue &other);

  SpeedValue operator-(double other);

  SpeedValue operator*=(const SpeedValue &other);

  SpeedValue operator*=(double other);

  SpeedValue operator/=(const SpeedValue &other);

  SpeedValue operator/=(double other);

  SpeedValue operator+=(const SpeedValue &other);

  SpeedValue operator+=(double other);

  SpeedValue operator-=(const SpeedValue &other);

  SpeedValue operator-=(double other);

  /**
   * Allows for printing an instance of a SpeedValue to the console using Serial.print().
   *
   * @param p the printer
   * @return the size of the text printed
   */
  size_t printTo(Print &p) const override {
    size_t r = 0;
    r += p.print("SpeedValue: ");
    r += p.print(value);
    r += p.print(" ");
    r += p.print(unitNames[_speedUnit - 1]);
    return r;
  };

};

#define SpeedNative(value)    SpeedValue(value, NATIVE)
#define SpeedPercent(value)   SpeedValue(value, PERCENT)
#define SpeedRPS(value)       SpeedValue(value, RPS)
#define SpeedRPM(value)       SpeedValue(value, RPM)
#define SpeedDPS(value)       SpeedValue(value, DPS)
#define SpeedDPM(value)       SpeedValue(value, DPM)

#endif //REDCROSS_SPEEDVALUE_H
